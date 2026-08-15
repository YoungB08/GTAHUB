/**
 * GTAHUB Custom Nametag Overlay Client Logic
 * 3D-to-2D Player Head Projection & Modern Glassmorphism UI
 */

(function () {
    const container = document.getElementById('nametags-container');
    const playerNametags = new Map();
    let localPlayerId = -1;
    let showLocalNametag = true;
    let cameraData = null;

    // Helper: Convert ARGB integer to CSS hex/rgba (#AARRGGBB -> rgba(r,g,b,a))
    function argbToHex(argb) {
        if (!argb || argb === -1) return '#ffffff';
        const u = argb >>> 0;
        const a = ((u >> 24) & 0xFF) / 255;
        const r = (u >> 16) & 0xFF;
        const g = (u >> 8) & 0xFF;
        const b = u & 0xFF;
        return `rgba(${r}, ${g}, ${b}, ${a > 0 ? a : 1})`;
    }

    // Helper: Preset Role Badges mapping with icons
    const PRESET_ROLES = {
        1: { text: 'ADMIN', class: 'role-admin', icon: 'http://gtahub.kntech.co/icons/admin.png' },
        2: { text: 'VIP', class: 'role-vip', icon: 'http://gtahub.kntech.co/icons/vip.png' },
        3: { text: 'MODERATOR', class: 'role-mod', icon: 'http://gtahub.kntech.co/icons/moderator.png' },
        4: { text: 'HELPER', class: 'role-helper', icon: 'http://gtahub.kntech.co/icons/helper.png' },
        5: { text: 'DEVELOPER', class: 'role-dev', icon: 'http://gtahub.kntech.co/icons/dev.png' }
    };

    // Smooth 60 FPS requestAnimationFrame render loop
    function renderLoop() {
        playerNametags.forEach((card) => {
            if (card.style.display === 'none') return;

            const targetX = parseFloat(card.dataset.targetX || 0);
            const targetY = parseFloat(card.dataset.targetY || 0);
            const targetScale = parseFloat(card.dataset.targetScale || 1);

            const currentX = parseFloat(card.dataset.currentX || targetX);
            const currentY = parseFloat(card.dataset.currentY || targetY);
            const currentScale = parseFloat(card.dataset.currentScale || targetScale);

            const lerpX = currentX + (targetX - currentX) * 0.90;
            const lerpY = currentY + (targetY - currentY) * 0.90;
            const lerpScale = currentScale + (targetScale - currentScale) * 0.90;

            card.dataset.currentX = lerpX;
            card.dataset.currentY = lerpY;
            card.dataset.currentScale = lerpScale;

            card.style.left = `${lerpX}px`;
            card.style.top = `${lerpY}px`;
            card.style.transform = `translate(-50%, -100%) scale(${lerpScale})`;
        });

        requestAnimationFrame(renderLoop);
    }
    requestAnimationFrame(renderLoop);

    // 3D World to 2D Screen Projection
    function project3DToScreen(worldX, worldY, worldZ, cam) {
        if (!cam || typeof cam.fx !== 'number') return null;

        const dx = worldX - cam.cx;
        const dy = worldY - cam.cy;
        const dz = worldZ - cam.cz;

        // Front vector dot product (distance along view axis)
        const forward = dx * cam.fx + dy * cam.fy + dz * cam.fz;
        if (forward <= 0.2) return null; // Target is behind camera

        // Calculate Right Vector (Cross product of Front vector with World Up [0, 0, 1])
        let rx = -cam.fy;
        let ry = cam.fx;
        let rz = 0;
        const rLen = Math.hypot(rx, ry);
        if (rLen > 0.0001) { rx /= rLen; ry /= rLen; }

        // Calculate Up Vector (Cross product of Right vector with Front vector)
        const ux = ry * cam.fz - rz * cam.fy;
        const uy = rz * cam.fx - rx * cam.fz;
        const uz = rx * cam.fy - ry * cam.fx;

        const distR = dx * rx + dy * ry + dz * rz;
        const distU = dx * ux + dy * uy + dz * uz;

        const halfH = window.innerHeight / 2;
        const halfW = window.innerWidth / 2;
        const fovScale = halfH * 1.35; // Standard SA-MP FOV projection ratio

        const screenX = halfW + (distR / forward) * fovScale;
        const screenY = halfH - (distU / forward) * fovScale;

        // Check if on screen bounds
        if (screenX < -200 || screenX > window.innerWidth + 200 || screenY < -200 || screenY > window.innerHeight + 200) {
            return null;
        }

        return {
            x: screenX,
            y: screenY,
            scale: Math.max(0.6, Math.min(1.1, 1.0 - (forward / 60.0)))
        };
    }

    function createNametagElement(id) {
        const card = document.createElement('div');
        card.className = 'nametag-card';
        card.id = `nametag-${id}`;

        card.innerHTML = `
            <div class="role-slots-wrapper"></div>
            <div class="nametag-header">
                <span class="local-player-tag" style="display: none;">YOU</span>
                <span class="nametag-name">Player</span>
                <span class="nametag-id">#${id}</span>
            </div>
            <div class="stat-bars-container">
                <div class="bar-wrapper"><div class="bar-fill health" style="width: 100%;"></div></div>
                <div class="bar-wrapper armour-bar" style="display: none;"><div class="bar-fill armour" style="width: 0%;"></div></div>
            </div>
        `;
        container.appendChild(card);
        return card;
    }

    function updatePlayerNametag(data) {
        const {
            id,
            name = `Player_${id}`,
            x = 0,
            y = 0,
            z = 0,
            visible = true,
            health = 100,
            armour = 0,
            color = -1,
            isLocal = false,
            rainbow = false,
            roles = []
        } = data;

        if (isLocal && !showLocalNametag) {
            removeNametag(id);
            return;
        }

        let card = playerNametags.get(id);
        if (!card) {
            card = createNametagElement(id);
            playerNametags.set(id, card);
        }

        if (!visible) {
            card.style.display = 'none';
            return;
        }

        // Project 3D coordinates (x, y, headZ) to 2D Screen Position
        let proj = null;
        if (x !== 0 || y !== 0 || z !== 0) {
            proj = project3DToScreen(x, y, z, cameraData);
        }

        if (!proj) {
            card.style.display = 'none';
            return;
        }

        card.style.display = 'flex';
        card.dataset.targetX = proj.x;
        card.dataset.targetY = proj.y;
        card.dataset.targetScale = proj.scale;

        // If card was just created or hidden, set initial position immediately
        if (!card.dataset.currentX) {
            card.dataset.currentX = proj.x;
            card.dataset.currentY = proj.y;
            card.dataset.currentScale = proj.scale;
            card.style.left = `${proj.x}px`;
            card.style.top = `${proj.y}px`;
            card.style.transform = `translate(-50%, -100%) scale(${proj.scale})`;
        }

        // Update Header & Player Name
        const nameEl = card.querySelector('.nametag-name');
        nameEl.textContent = name;
        if (rainbow) {
            nameEl.classList.add('rainbow-text');
            nameEl.style.color = '';
        } else {
            nameEl.classList.remove('rainbow-text');
            nameEl.style.color = argbToHex(color);
        }

        // Update ID Tag
        const idEl = card.querySelector('.nametag-id');
        idEl.textContent = `#${id}`;

        // Update Local Player Tag
        const localTag = card.querySelector('.local-player-tag');
        localTag.style.display = isLocal ? 'inline-block' : 'none';

        // Update Health & Armour
        const hpFill = card.querySelector('.bar-fill.health');
        hpFill.style.width = `${Math.max(0, Math.min(100, health))}%`;

        const armBar = card.querySelector('.armour-bar');
        const armFill = card.querySelector('.bar-fill.armour');
        if (armour > 0) {
            armBar.style.display = 'block';
            armFill.style.width = `${Math.max(0, Math.min(100, armour))}%`;
        } else {
            armBar.style.display = 'none';
        }

        // Update Multi-Slot Roles (Cached to prevent layout thrashing)
        const rolesSig = JSON.stringify(roles || []);
        if (card.dataset.rolesSig !== rolesSig) {
            card.dataset.rolesSig = rolesSig;
            const roleWrapper = card.querySelector('.role-slots-wrapper');
            roleWrapper.innerHTML = '';

            if (Array.isArray(roles) && roles.length > 0) {
                roles.forEach(role => {
                    if (!role) return;
                    const badge = document.createElement('span');
                    badge.className = 'role-badge';

                let presetObj = null;
                let roleObj = (typeof role === 'object') ? role : null;

                if (typeof role === 'number') {
                    presetObj = PRESET_ROLES[role];
                } else if (roleObj && roleObj.preset) {
                    presetObj = PRESET_ROLES[roleObj.preset];
                }

                if (presetObj) {
                    badge.classList.add(presetObj.class);
                }
                if (roleObj && roleObj.color) {
                    badge.style.backgroundColor = argbToHex(roleObj.color);
                }

                const iconUrl = (roleObj && roleObj.icon) || (presetObj && presetObj.icon);
                const textStr = (roleObj && roleObj.text) || (presetObj && presetObj.text) || (typeof role === 'string' ? role : '');

                if (iconUrl) {
                    const img = document.createElement('img');
                    img.src = iconUrl;
                    img.alt = textStr || 'icon';
                    badge.appendChild(img);
                }

                if (textStr) {
                    const txt = document.createElement('span');
                    txt.textContent = textStr;
                    if (roleObj && roleObj.rainbow) txt.classList.add('rainbow-text');
                    badge.appendChild(txt);
                }

                roleWrapper.appendChild(badge);
            });
        }
    }

    function removeNametag(id) {
        const card = playerNametags.get(id);
        if (card) {
            card.remove();
            playerNametags.delete(id);
        }
    }

    // CEF Event Listeners
    if (window.cef) {
        cef.on('GTAHUB:UpdateNametags', (jsonInput) => {
            try {
                const payload = typeof jsonInput === 'string' ? JSON.parse(jsonInput) : jsonInput;
                if (payload.cam) {
                    cameraData = payload.cam;
                }
                const players = payload.players || payload;
                if (Array.isArray(players)) {
                    const activeIds = new Set(players.map(p => p.id));
                    playerNametags.forEach((card, id) => {
                        if (!activeIds.has(id)) {
                            removeNametag(id);
                        }
                    });
                    players.forEach(updatePlayerNametag);
                }
            } catch (e) {
                console.error('[CEF Nametag] Failed to parse UpdateNametags JSON:', e);
            }
        });

        cef.on('GTAHUB:SetLocalPlayerId', (id) => {
            localPlayerId = id;
        });

        cef.on('GTAHUB:ToggleLocalNametag', (toggle) => {
            showLocalNametag = !!toggle;
        });

        cef.on('GTAHUB:RemoveNametag', (id) => {
            removeNametag(id);
        });
    }

    // Embedded Preview Mode for iframe testing
    if (window.location.search.includes('embed')) {
        updatePlayerNametag({
            id: 0,
            name: 'Patrick_Dave',
            x: 0, y: 0, z: 0,
            visible: true,
            health: 85,
            armour: 60,
            color: 0xFF00FFCC,
            isLocal: true,
            roles: [
                { preset: 1 },
                { preset: 2 },
                { text: 'DEV TEAM', icon: 'http://gtahub.kntech.co/icons/dev.png', color: 0xFF7C3AED }
            ]
        });
    }
})();
