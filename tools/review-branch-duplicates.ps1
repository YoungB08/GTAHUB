<#
.SYNOPSIS
Reviews duplicate commits across branches, excluding commits already in main by default.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tools/review-branch-duplicates.ps1

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tools/review-branch-duplicates.ps1 -IncludeRemotes

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tools/review-branch-duplicates.ps1 -BaseBranch main -Json
#>

[CmdletBinding()]
param(
    [string]$BaseBranch = "main",
    [string[]]$Branches = @(),
    [switch]$IncludeRemotes,
    [switch]$IncludeMerges,
    [switch]$NoPatchId,
    [switch]$Json,
    [int]$MaxGroups = 50
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-Git {
    param([string[]]$GitArgs)

    $output = & git @GitArgs 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "git $($GitArgs -join ' ') failed:`n$output"
    }
    return @($output)
}

function Resolve-BaseRef {
    param([string]$Name)

    try {
        [void](Invoke-Git @("rev-parse", "--verify", "$Name^{commit}"))
        return $Name
    }
    catch {
        $remoteName = "origin/$Name"
        try {
            [void](Invoke-Git @("rev-parse", "--verify", "$remoteName^{commit}"))
            return $remoteName
        }
        catch {
            throw "Cannot resolve base branch '$Name' or '$remoteName'."
        }
    }
}

function Get-ReviewBranches {
    param(
        [string]$BaseName,
        [string[]]$SelectedBranches,
        [switch]$WithRemotes
    )

    if ($SelectedBranches.Count -gt 0) {
        return @($SelectedBranches | Where-Object { $_ -and $_.Trim() } | Sort-Object -Unique)
    }

    $refs = @(Invoke-Git @("for-each-ref", "--format=%(refname:short)", "refs/heads"))
    if ($WithRemotes) {
        $refs += @(Invoke-Git @("for-each-ref", "--format=%(refname:short)", "refs/remotes"))
    }

    return @(
        $refs |
            Where-Object {
                $_ -and
                $_ -notmatch "/HEAD$" -and
                $_ -ne $BaseName -and
                $_ -notmatch ("(^|/)" + [regex]::Escape($BaseName) + "$")
            } |
            Sort-Object -Unique
    )
}

function Get-BranchCommits {
    param(
        [string]$Branch,
        [string]$BaseRef,
        [switch]$WithMerges
    )

    $args = @("log", "--format=%H%x1f%h%x1f%an%x1f%ai%x1f%s")
    if (-not $WithMerges) {
        $args += "--no-merges"
    }
    $args += @($Branch, "--not", $BaseRef)

    $rows = @(Invoke-Git $args)
    foreach ($row in $rows) {
        if ([string]::IsNullOrWhiteSpace($row)) {
            continue
        }

        $parts = $row -split ([char]0x1f), 5
        if ($parts.Count -lt 5) {
            continue
        }

        [pscustomobject]@{
            Branch  = $Branch
            Hash    = $parts[0]
            Short   = $parts[1]
            Author  = $parts[2]
            Date    = $parts[3]
            Subject = $parts[4]
            PatchId = $null
        }
    }
}

function Get-StablePatchId {
    param([string]$CommitHash)

    $patch = @(& git show --format= --no-ext-diff --binary $CommitHash 2>$null)
    if ($LASTEXITCODE -ne 0 -or $patch.Count -eq 0) {
        return $null
    }

    $patchIdOutput = @($patch | & git patch-id --stable 2>$null)
    if ($LASTEXITCODE -ne 0 -or $patchIdOutput.Count -eq 0) {
        return $null
    }

    $firstLine = [string]($patchIdOutput | Select-Object -First 1)
    if ([string]::IsNullOrWhiteSpace($firstLine)) {
        return $null
    }

    return (($firstLine -split "\s+") | Select-Object -First 1)
}

function Convert-ToDuplicateGroup {
    param(
        [string]$KeyName,
        [string]$KeyValue,
        [object[]]$Commits
    )

    $branches = @($Commits.Branch | Sort-Object -Unique)
    $hashes = @($Commits.Hash | Sort-Object -Unique)

    [pscustomobject]@{
        Type     = $KeyName
        Key      = $KeyValue
        Branches = $branches
        Hashes   = $hashes
        Commits  = @(
            $Commits |
                Sort-Object Branch, Date, Hash |
                Select-Object Branch, Short, Hash, Author, Date, Subject
        )
    }
}

function Write-DuplicateGroups {
    param(
        [string]$Title,
        [object[]]$Groups,
        [int]$Limit
    )

    Write-Output ""
    Write-Output "## $Title"

    if ($Groups.Count -eq 0) {
        Write-Output "Khong tim thay."
        return
    }

    $shown = 0
    foreach ($group in ($Groups | Select-Object -First $Limit)) {
        $shown++
        Write-Output ""
        Write-Output "$shown. $($group.Type): $($group.Key)"
        Write-Output "   Branches: $($group.Branches -join ', ')"

        foreach ($commit in $group.Commits) {
            Write-Output "   - [$($commit.Branch)] $($commit.Short) $($commit.Subject)"
        }
    }

    if ($Groups.Count -gt $Limit) {
        Write-Output ""
        Write-Output "... an bot $($Groups.Count - $Limit) nhom. Tang -MaxGroups neu can xem them."
    }
}

$baseRef = Resolve-BaseRef $BaseBranch
$reviewBranches = @(Get-ReviewBranches -BaseName $BaseBranch -SelectedBranches $Branches -WithRemotes:$IncludeRemotes)

if ($reviewBranches.Count -eq 0) {
    throw "No branches to review after excluding '$BaseBranch'."
}

$records = New-Object System.Collections.Generic.List[object]
$seenBranchCommit = @{}

foreach ($branch in $reviewBranches) {
    foreach ($commit in @(Get-BranchCommits -Branch $branch -BaseRef $baseRef -WithMerges:$IncludeMerges)) {
        $seenKey = "$($commit.Branch)|$($commit.Hash)"
        if ($seenBranchCommit.ContainsKey($seenKey)) {
            continue
        }
        $seenBranchCommit[$seenKey] = $true
        $records.Add($commit)
    }
}

if (-not $NoPatchId) {
    $patchCache = @{}
    foreach ($commit in $records) {
        if (-not $patchCache.ContainsKey($commit.Hash)) {
            $patchCache[$commit.Hash] = Get-StablePatchId $commit.Hash
        }
        $commit.PatchId = $patchCache[$commit.Hash]
    }
}

$exactDuplicates = @(
    $records |
        Group-Object Hash |
        Where-Object { @($_.Group.Branch | Sort-Object -Unique).Count -gt 1 } |
        ForEach-Object { Convert-ToDuplicateGroup -KeyName "same-sha" -KeyValue $_.Name -Commits $_.Group }
)

$patchDuplicates = @()
if (-not $NoPatchId) {
    $patchDuplicates = @(
        $records |
            Where-Object { $_.PatchId } |
            Group-Object PatchId |
            Where-Object {
                @($_.Group.Hash | Sort-Object -Unique).Count -gt 1 -and
                @($_.Group.Branch | Sort-Object -Unique).Count -gt 1
            } |
            ForEach-Object { Convert-ToDuplicateGroup -KeyName "same-patch" -KeyValue $_.Name -Commits $_.Group }
    )
}

$result = [pscustomobject]@{
    BaseBranch          = $baseRef
    BranchesReviewed    = $reviewBranches
    CommitsReviewed     = $records.Count
    ExactDuplicateCount = $exactDuplicates.Count
    PatchDuplicateCount = $patchDuplicates.Count
    ExactDuplicates     = $exactDuplicates
    PatchDuplicates     = $patchDuplicates
}

if ($Json) {
    $result | ConvertTo-Json -Depth 8
    exit 0
}

Write-Output "# Branch Duplicate Commit Review"
Write-Output "Base excluded: $baseRef"
Write-Output "Branches reviewed: $($reviewBranches -join ', ')"
Write-Output "Commits reviewed: $($records.Count)"
Write-Output "Exact duplicate SHA groups: $($exactDuplicates.Count)"
Write-Output "Duplicate patch groups: $($patchDuplicates.Count)"

Write-DuplicateGroups -Title "Trung SHA commit" -Groups $exactDuplicates -Limit $MaxGroups
Write-DuplicateGroups -Title "Trung noi dung cherry-pick" -Groups $patchDuplicates -Limit $MaxGroups
