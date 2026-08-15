/**
 * GTAHUB Custom Nametag - World2D Browser Engine (JS)
 * Position & 3D World Projection is managed 100% natively by C++ samp-cef Plugin
 */

(function () {
    'use strict';

    const card = document.querySelector('.nametag-card') || document.body;

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

    if (window.cef) {
        cef.on('GTAHUB:SetNametagData', (id, name, hp, arm, color, isLocal, rainbow, rolesInput, isVoice, isAFK) => {
            const nameEl = document.querySelector('.nametag-name');
            if (nameEl) {
                nameEl.textContent = name || `Player_${id}`;
                if (rainbow) {
                    nameEl.classList.add('rainbow-text');
                    nameEl.style.color = '';
                } else {
                    nameEl.classList.remove('rainbow-text');
                    nameEl.style.color = argbToHex(color);
                }
            }

            const idEl = document.querySelector('.nametag-id');
            if (idEl) idEl.textContent = `#${id}`;

            const localTag = document.querySelector('.local-player-tag');
            if (localTag) localTag.style.display = isLocal ? 'inline-block' : 'none';

            const voiceIcon = document.querySelector('.voice-icon');
            if (voiceIcon) voiceIcon.style.display = isVoice ? 'inline-block' : 'none';

            const afkIcon = document.querySelector('.afk-icon');
            if (afkIcon) afkIcon.style.display = isAFK ? 'inline-block' : 'none';

            const hpFill = document.querySelector('.bar-fill.health');
            if (hpFill) hpFill.style.width = `${clamp(hp, 0, 100)}%`;

            const armBar = document.querySelector('.armour-bar');
            const armFill = document.querySelector('.bar-fill.armour');
            if (armBar && armFill) {
                if (arm > 0) {
                    armBar.style.display = 'block';
                    armFill.style.width = `${clamp(arm, 0, 100)}%`;
                } else {
                    armBar.style.display = 'none';
                }
            }

            // Role Badges
            let roles = [];
            try {
                roles = typeof rolesInput === 'string' ? JSON.parse(rolesInput) : (rolesInput || []);
            } catch (e) {
                roles = [];
            }

            const rolesSig = JSON.stringify(roles);
            if (card.dataset.rolesSig !== rolesSig) {
                card.dataset.rolesSig = rolesSig;
                const roleWrapper = document.querySelector('.role-slots-wrapper');
                if (roleWrapper) {
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
        });
    }

})();
