/**
 * GTAHUB Custom Nametag Engine (World2D Client Mode)
 * Hardware-accelerated 3D->2D projection driven by native C++ CEF World2D plugin.
 */

(function () {
    'use strict';

    const roleWrapper = document.getElementById('role-slots-wrapper');
    const localTag = document.getElementById('local-player-tag');
    const voiceIcon = document.getElementById('voice-icon');
    const afkIcon = document.getElementById('afk-icon');
    const nameEl = document.getElementById('nametag-name');
    const idEl = document.getElementById('nametag-id');
    const hpFill = document.getElementById('health-fill');
    const armBar = document.getElementById('armour-bar');
    const armFill = document.getElementById('armour-fill');

    const PRESET_ROLES = {
        1: { text: 'ADMIN', class: 'role-admin', icon: 'http://gtahub.kntech.co/icons/admin.png' },
        2: { text: 'VIP', class: 'role-vip', icon: 'http://gtahub.kntech.co/icons/vip.png' },
        3: { text: 'MODERATOR', class: 'role-mod', icon: 'http://gtahub.kntech.co/icons/moderator.png' },
        4: { text: 'HELPER', class: 'role-helper', icon: 'http://gtahub.kntech.co/icons/helper.png' },
        5: { text: 'DEVELOPER', class: 'role-dev', icon: 'http://gtahub.kntech.co/icons/dev.png' },
        6: { text: 'MOD', class: 'role-mod', icon: 'http://gtahub.kntech.co/icons/mod.png' }
    };

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

    let lastRolesSig = '';

    function updateNametag(id, name, hp, arm, color, isLocal, rainbow, rolesRaw, isVoice, isAFK) {
        if (nameEl) nameEl.textContent = name || `Player_${id}`;
        if (idEl) idEl.textContent = `#${id}`;

        if (rainbow) {
            if (nameEl && !nameEl.classList.contains('rainbow-text')) nameEl.classList.add('rainbow-text');
            if (nameEl) nameEl.style.color = '';
        } else {
            if (nameEl && nameEl.classList.contains('rainbow-text')) nameEl.classList.remove('rainbow-text');
            if (nameEl) nameEl.style.color = argbToHex(color);
        }

        if (localTag) localTag.style.display = isLocal ? 'inline-block' : 'none';
        if (voiceIcon) voiceIcon.style.display = isVoice ? 'inline-block' : 'none';
        if (afkIcon) afkIcon.style.display = isAFK ? 'inline-block' : 'none';

        const healthVal = clamp(typeof hp === 'number' ? hp : 100, 0, 100);
        if (hpFill) hpFill.style.width = `${healthVal}%`;

        const armourVal = clamp(typeof arm === 'number' ? arm : 0, 0, 100);
        if (armBar && armFill) {
            if (armourVal > 0) {
                armBar.style.display = 'block';
                armFill.style.width = `${armourVal}%`;
            } else {
                armBar.style.display = 'none';
            }
        }

        // Roles Update
        let roles = [];
        try {
            roles = typeof rolesRaw === 'string' ? JSON.parse(rolesRaw) : rolesRaw;
        } catch (e) {
            roles = [];
        }

        const rolesSig = JSON.stringify(roles);
        if (lastRolesSig !== rolesSig && roleWrapper) {
            lastRolesSig = rolesSig;
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

    if (window.cef) {
        cef.on('GTAHUB:SetNametagData', (id, name, hp, arm, color, isLocal, rainbow, rolesRaw, isVoice, isAFK) => {
            updateNametag(id, name, hp, arm, color, isLocal, rainbow, rolesRaw, isVoice, isAFK);
        });
    }
})();
