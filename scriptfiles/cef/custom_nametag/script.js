/**
 * GTAHUB Custom Nametag Overlay Engine (JS)
 * - Single Fullscreen Transparent Browser Architecture
 * - Pure 3D World to 2D Screen Matrix Projection & Backface Culling
 * - 60-144 FPS requestAnimationFrame Linear Interpolation (Lerp)
 * - Dynamic Distance Scaling & Smooth Fade Out (25m - 35m)
 */

(function () {
    'use strict';

    const nametagsContainer = document.getElementById('nametags-container');
    const playerNametags = new Map(); // id -> DOM element

    let cameraData = { cx: 0, cy: 0, cz: 0, fx: 0, fy: 0, fz: 1 };
    let localPlayerId = -1;

    // Preset Role Configurations (Loaded from http://gtahub.kntech.co/icons/)
    const PRESET_ROLES = {
        1: { text: 'ADMIN', class: 'role-admin', icon: 'http://gtahub.kntech.co/icons/admin.png' },
        2: { text: 'VIP', class: 'role-vip', icon: 'http://gtahub.kntech.co/icons/vip.png' },
        3: { text: 'MODERATOR', class: 'role-mod', icon: 'http://gtahub.kntech.co/icons/moderator.png' },
        4: { text: 'HELPER', class: 'role-helper', icon: 'http://gtahub.kntech.co/icons/helper.png' },
        5: { text: 'DEVELOPER', class: 'role-dev', icon: 'http://gtahub.kntech.co/icons/dev.png' },
        6: { text: 'MOD', class: 'role-mod', icon: 'http://gtahub.kntech.co/icons/mod.png' }
    };

    // --- Helper Functions ---
    function argbToHex(argb) {
        if (!argb || argb === -1) return '#ffffff';
        const u = argb >>> 0;
        const r = (u >> 16) & 0xFF;
        const g = (u >> 8) & 0xFF;
        const b = u & 0xFF;
        return `#${((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1)}`;
    }

    function clamp(val, min, max) {
        return Math.max(min, Math.min(max, val));
    }

    // --- 3D World to 2D Screen Matrix Projection ---
    function project3DToScreen(worldX, worldY, worldZ, cam) {
        if (!cam || typeof cam.fx !== 'number') return null;

        const dx = worldX - cam.cx;
        const dy = worldY - cam.cy;
        const dz = worldZ - cam.cz;

        // Front Vector Dot Product (Depth check)
        const depth = dx * cam.fx + dy * cam.fy + dz * cam.fz;
        if (depth <= 0.15) {
            return null; // Backface / Behind camera culling
        }

        // Compute Right vector: Cross(Front, WorldUp[0, 0, 1])
        let rx = cam.fy * 1.0 - cam.fz * 0.0;
        let ry = cam.fz * 0.0 - cam.fx * 1.0;
        let rz = cam.fx * 0.0 - cam.fy * 0.0;

        let rLen = Math.hypot(rx, ry, rz);
        if (rLen < 0.0001) {
            rx = 1.0; ry = 0.0; rz = 0.0;
        } else {
            rx /= rLen; ry /= rLen; rz /= rLen;
        }

        // Compute Up vector: Cross(Right, Front)
        const ux = ry * cam.fz - rz * cam.fy;
        const uy = rz * cam.fx - rx * cam.fz;
        const uz = rx * cam.fy - ry * cam.fx;

        // Project relative vector to local camera space
        const localX = dx * rx + dy * ry + dz * rz;
        const localY = dx * ux + dy * uy + dz * uz;

        const fovFactor = 1.15; // Standard SA-MP Field of View scale factor
        const screenWidth = window.innerWidth || 1920;
        const screenHeight = window.innerHeight || 1080;

        const projectedX = (screenWidth / 2) + (localX / depth) * (screenHeight / 2) * fovFactor;
        const projectedY = (screenHeight / 2) - (localY / depth) * (screenHeight / 2) * fovFactor;

        // Calculate distance-based scaling & opacity fade
        const dist = Math.hypot(dx, dy, dz);
        const distScale = clamp(1.0 - (dist / 35.0) * 0.40, 0.60, 1.0);
        let opacity = 1.0;

        if (dist > 25.0) {
            opacity = clamp(1.0 - (dist - 25.0) / 10.0, 0.0, 1.0);
        }

        return {
            x: projectedX,
            y: projectedY,
            scale: distScale,
            opacity: opacity
        };
    }

    // --- Create Nametag DOM Element ---
    function createNametagElement(id) {
        const card = document.createElement('div');
        card.className = 'nametag-card';
        card.dataset.id = id;

        card.innerHTML = `
            <div class="role-slots-wrapper"></div>
            <div class="nametag-header">
                <span class="local-player-tag" style="display: none;">YOU</span>
                <span class="indicator-icon voice-icon" style="display: none;" title="Voice Speaking">🎙️</span>
                <span class="indicator-icon afk-icon" style="display: none;" title="AFK">💤</span>
                <span class="nametag-name">Player</span>
                <span class="nametag-id">#${id}</span>
            </div>
            <div class="stat-bars-container">
                <div class="bar-wrapper health-bar">
                    <div class="bar-fill health" style="width: 100%;"></div>
                </div>
                <div class="bar-wrapper armour-bar" style="display: none;">
                    <div class="bar-fill armour" style="width: 0%;"></div>
                </div>
            </div>
        `;

        nametagsContainer.appendChild(card);
        return card;
    }

    // --- Update Single Player Nametag Data ---
    function updatePlayerNametag(data) {
        if (!data || typeof data.id !== 'number') return;

        const id = data.id;
        const name = data.name || `Player_${id}`;
        const health = typeof data.hp === 'number' ? data.hp : (data.health || 100);
        const armour = typeof data.arm === 'number' ? data.arm : (data.armour || 0);
        const color = data.color || -1;
        const isLocal = !!data.local || !!data.isLocal;
        const rainbow = !!data.rainbow;
        const roles = data.roles || [];
        const isVoice = !!data.voice;
        const isAFK = !!data.afk;
        const x = data.x || 0;
        const y = data.y || 0;
        const z = data.z || 0;

        let card = playerNametags.get(id);
        if (!card) {
            card = createNametagElement(id);
            playerNametags.set(id, card);
        }

        // Screen Projection
        const proj = project3DToScreen(x, y, z, cameraData);
        if (!proj || proj.opacity <= 0.01) {
            card.style.display = 'none';
            return;
        }

        card.style.display = 'flex';
        card.dataset.targetX = proj.x;
        card.dataset.targetY = proj.y;
        card.dataset.targetScale = proj.scale;
        card.dataset.targetOpacity = proj.opacity;

        if (!card.dataset.currentX) {
            card.dataset.currentX = proj.x;
            card.dataset.currentY = proj.y;
            card.dataset.currentScale = proj.scale;
            card.dataset.currentOpacity = proj.opacity;
        }

        // Header & Name
        const nameEl = card.querySelector('.nametag-name');
        nameEl.textContent = name;
        if (rainbow) {
            nameEl.classList.add('rainbow-text');
            nameEl.style.color = '';
        } else {
            nameEl.classList.remove('rainbow-text');
            nameEl.style.color = argbToHex(color);
        }

        // ID Tag & Indicators
        const idEl = card.querySelector('.nametag-id');
        idEl.textContent = `#${id}`;

        const localTag = card.querySelector('.local-player-tag');
        localTag.style.display = isLocal ? 'inline-block' : 'none';

        const voiceIcon = card.querySelector('.voice-icon');
        voiceIcon.style.display = isVoice ? 'inline-block' : 'none';

        const afkIcon = card.querySelector('.afk-icon');
        afkIcon.style.display = isAFK ? 'inline-block' : 'none';

        // HP & Armour Bars
        const hpFill = card.querySelector('.bar-fill.health');
        hpFill.style.width = `${clamp(health, 0, 100)}%`;

        const armBar = card.querySelector('.armour-bar');
        const armFill = card.querySelector('.bar-fill.armour');
        if (armour > 0) {
            armBar.style.display = 'block';
            armFill.style.width = `${clamp(armour, 0, 100)}%`;
        } else {
            armBar.style.display = 'none';
        }

        // Multi-Slot Roles (Cached Signature)
        const rolesSig = JSON.stringify(roles);
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

                    if (presetObj) badge.classList.add(presetObj.class);
                    if (roleObj && roleObj.color) badge.style.backgroundColor = argbToHex(roleObj.color);

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
    }

    function removeNametag(id) {
        const card = playerNametags.get(id);
        if (card) {
            card.remove();
            playerNametags.delete(id);
        }
    }

    // --- Smooth 60-144 FPS requestAnimationFrame Render Loop ---
    function renderLoop() {
        playerNametags.forEach((card) => {
            if (card.style.display === 'none') return;

            const targetX = parseFloat(card.dataset.targetX || 0);
            const targetY = parseFloat(card.dataset.targetY || 0);
            const targetScale = parseFloat(card.dataset.targetScale || 1);
            const targetOpacity = parseFloat(card.dataset.targetOpacity || 1);

            const currentX = parseFloat(card.dataset.currentX || targetX);
            const currentY = parseFloat(card.dataset.currentY || targetY);
            const currentScale = parseFloat(card.dataset.currentScale || targetScale);
            const currentOpacity = parseFloat(card.dataset.currentOpacity || targetOpacity);

            // Responsive 0.85 Lerp for ultra-smooth 60-144 FPS tracking
            const lerpX = currentX + (targetX - currentX) * 0.85;
            const lerpY = currentY + (targetY - currentY) * 0.85;
            const lerpScale = currentScale + (targetScale - currentScale) * 0.85;
            const lerpOpacity = currentOpacity + (targetOpacity - currentOpacity) * 0.85;

            card.dataset.currentX = lerpX;
            card.dataset.currentY = lerpY;
            card.dataset.currentScale = lerpScale;
            card.dataset.currentOpacity = lerpOpacity;

            card.style.left = `${lerpX}px`;
            card.style.top = `${lerpY}px`;
            card.style.opacity = lerpOpacity.toFixed(2);
            card.style.transform = `translate(-50%, -100%) scale(${lerpScale.toFixed(3)})`;
        });

        requestAnimationFrame(renderLoop);
    }
    requestAnimationFrame(renderLoop);

    // --- CEF Event Listeners ---
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

        cef.on('GTAHUB:RemoveNametag', (id) => {
            removeNametag(id);
        });
    }

})();
