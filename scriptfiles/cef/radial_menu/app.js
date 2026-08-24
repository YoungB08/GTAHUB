(function () {
  const RadialMenu = {
    state: {
      targetId: 27,
      selectedSectorIndex: 0,
      selectedSubmenuIndex: -1,
      activeGroup: null,
      submenuOpen: false,
      isHoldMode: false,
      lastActionTime: 0,
    },

    // Default fallback groups (Overridden dynamically via setSectors)
    groups: {
      citizen: {
        title: 'CÔNG DÂN',
        icon: '../shared/svg/id-card.svg',
        items: [
          { label: 'Đưa tiền', action: 'radial.finance.giveMoney' },
          { label: 'Yêu cầu thanh toán', action: 'radial.finance.requestPayment' },
          { label: 'Xem ID', action: 'radial.documents.showID' },
          { label: 'Xuất trình bằng lái', action: 'radial.documents.showDriverLicense' },
          { label: 'Chào hỏi', action: 'radial.inspect.greet' }
        ]
      },
      blips: {
        title: 'ĐIỂM ĐÁNH DẤU',
        icon: '../shared/svg/inspect.svg',
        items: [
          { label: 'Đồn cảnh sát', action: 'radial.blips.policeStation' },
          { label: 'Bệnh viện', action: 'radial.blips.hospital' },
          { label: 'Nhà để xe', action: 'radial.blips.garage' },
          { label: 'Ngân hàng', action: 'radial.blips.bank' }
        ]
      },
      general: {
        title: 'TỔNG QUÁT',
        icon: '../shared/svg/vehicle.svg',
        items: [
          { label: 'Trao vật phẩm', action: 'radial.inventory.giveItem' },
          { label: 'Mở cốp xe', action: 'radial.vehicle.openTrunk' },
          { label: 'Mời lên xe', action: 'radial.vehicle.inviteVehicle' },
          { label: 'Chia sẻ chìa khóa', action: 'radial.vehicle.shareKey' },
          { label: 'Mời vào tổ đội', action: 'radial.inspect.partyInvite' }
        ]
      },
      police: {
        title: 'HÀNH ĐỘNG CẢNH SÁT',
        icon: '../shared/svg/police.svg',
        items: [
          { label: 'Còng tay', action: 'radial.action.cuff' },
          { label: 'Tháo còng', action: 'radial.action.uncuff' },
          { label: 'Khám người', action: 'radial.inspect.inspectPerson' },
          { label: 'Lập biên bản phạt', action: 'radial.action.fine' },
          { label: 'Đưa lên xe', action: 'radial.action.putInVehicle' }
        ]
      }
    },

    sectors: ['citizen', 'blips', 'general', 'police'],
    sectorPositions: ['sector-top', 'sector-right', 'sector-bottom', 'sector-left'],

    init() {
      this.cacheElements();
      this.bindEvents();
      this.selectSector(0, false);

      if (window.FocusManager) {
        window.FocusManager.request('radialMenu', () => this.close());
      }

      if (window.onCEF) {
        // Nhận dữ liệu phân khu động từ Server
        window.onCEF('radial.setSectors', (data) => {
          this.setSectors(data);
        });

        // Mở Radial Menu với dữ liệu dynamic
        window.onCEF('radial.open', (payload) => {
          const data = typeof payload === 'object' && payload !== null ? payload : { targetId: payload };
          if (Number.isFinite(Number(data.targetId))) this.state.targetId = Number(data.targetId);
          const label = data.targetLabel || data.targetName || `NGƯỜI CHƠI #${this.state.targetId}`;
          const targetLabel = document.getElementById('targetLabel');
          if (targetLabel) targetLabel.textContent = label;

          if (data.groups && typeof data.groups === 'object') {
            this.setSectors(data.groups);
          }
        });
      }

      // Export hàm cho local browser console test
      window.setRadialSectors = (groups) => this.setSectors(groups);
    },

    cacheElements() {
      this.elements = {
        mainRing: document.getElementById('mainRing'),
        sectors: document.querySelectorAll('.radial-sector'),
        submenuLayer: document.getElementById('submenuLayer'),
        submenuList: document.getElementById('submenuList'),
        radialWheel: document.getElementById('radialWheel'),
      };
    },

    /**
     * Hàm dựng động phân khu & danh sách tính năng khi click phân khu
     * @param {Object} groupsPayload - Danh sách các phân khu truyền từ Pawn / JS
     */
    setSectors(groupsPayload) {
      if (!groupsPayload || typeof groupsPayload !== 'object') return;

      let parsedPayload = groupsPayload;
      if (typeof groupsPayload === 'string') {
        try { parsedPayload = JSON.parse(groupsPayload); } catch (e) { return; }
      }

      this.groups = {};
      this.sectors = [];

      const keys = Object.keys(parsedPayload);
      keys.forEach((key, index) => {
        const item = parsedPayload[key];
        this.sectors.push(key);
        this.groups[key] = {
          title: item.title || item.label || key.toUpperCase(),
          icon: item.icon || '../shared/svg/inspect.svg',
          items: Array.isArray(item.items) ? item.items : []
        };
      });

      this.renderMainSectors();
      this.selectSector(0, false);
    },

    /**
     * Render lại HTML 4 phân khu hình vòng tròn động
     */
    renderMainSectors() {
      if (!this.elements.mainRing) return;

      const sectorHtml = this.sectors.map((key, index) => {
        const group = this.groups[key];
        const positionClass = this.sectorPositions[index % this.sectorPositions.length];
        const iconHtml = group.icon ? `<img src="${group.icon}" alt="">` : '';
        return `
          <button class="radial-sector ${positionClass}${index === this.state.selectedSectorIndex ? ' is-selected' : ''}" data-group="${key}" type="button">
            <span class="sector-content">${iconHtml}<strong>${group.title}</strong></span>
          </button>
        `;
      }).join('');

      this.elements.mainRing.innerHTML = sectorHtml;
      this.elements.sectors = this.elements.mainRing.querySelectorAll('.radial-sector');
      this.bindEvents();
    },

    bindEvents() {
      this.elements.sectors.forEach((sector, index) => {
        sector.addEventListener('mouseenter', () => this.selectSector(index));
        sector.addEventListener('click', () => this.handleSectorClick(index));
      });

      if (!this.pointerBound) {
        document.addEventListener('pointerdown', (event) => {
          if (this.state.submenuOpen && !event.target.closest('.radial-wheel')) this.closeSubmenu();
        });
        window.addEventListener('keydown', (event) => this.handleKeyDown(event));
        window.addEventListener('keyup', (event) => {
          if ((event.key === 'Alt' || event.button === 1) && this.state.isHoldMode && this.state.selectedSubmenuIndex === -1) {
            this.close();
          }
        });
        this.pointerBound = true;
      }
    },

    selectSector(index, open = this.state.submenuOpen) {
      if (!this.sectors[index]) return;
      this.state.selectedSectorIndex = index;
      this.state.selectedSubmenuIndex = -1;
      this.state.activeGroup = this.sectors[index];
      this.elements.sectors.forEach((sector, idx) => sector.classList.toggle('is-selected', idx === index));

      const group = this.groups[this.state.activeGroup];
      if (!group) return;
      if (open) {
        this.state.submenuOpen = true;
        this.state.selectedSubmenuIndex = 0;
        this.elements.radialWheel.classList.add('submenu-open');
        this.renderSubmenu(group.items);
      } else {
        this.closeSubmenu();
      }
    },

    handleSectorClick(index) {
      this.selectSector(index, false);
      const sectorKey = this.sectors[index];
      // Báo sự kiện click phân khu cho Server
      window.emitCEF('radial.sectorClick', sectorKey, this.state.targetId);
      this.openSubmenu();
    },

    openSubmenu() {
      const group = this.groups[this.state.activeGroup];
      if (!group) return;
      this.state.submenuOpen = true;
      this.state.selectedSubmenuIndex = 0;
      this.elements.radialWheel.classList.add('submenu-open');
      this.renderSubmenu(group.items);
      window.emitCEF('radial.group.open', this.state.activeGroup, this.state.targetId);
    },

    closeSubmenu() {
      this.state.submenuOpen = false;
      this.state.selectedSubmenuIndex = -1;
      this.elements.radialWheel.classList.remove('submenu-open');
      if (this.elements.submenuLayer) {
        this.elements.submenuLayer.classList.add('hidden');
        this.elements.submenuLayer.classList.remove('is-open');
      }
    },

    renderSubmenu(items) {
      if (!this.elements.submenuLayer || !this.elements.submenuList) return;
      this.elements.submenuLayer.classList.remove('hidden');
      this.elements.submenuLayer.classList.add('is-open');

      if (!items || items.length === 0) {
        this.elements.submenuList.innerHTML = '<div class="submenu-empty">Khong co tinh nang</div>';
        return;
      }

      const radius = 224;
      const count = items.length;

      this.elements.submenuList.innerHTML = items.map((item, index) => {
        const angle = (index / count) * (2 * Math.PI) - Math.PI / 2;
        const x = Math.round(radius * Math.cos(angle));
        const y = Math.round(radius * Math.sin(angle));
        return `<button class="submenu-item${index === this.state.selectedSubmenuIndex ? ' is-active' : ''}" type="button" data-idx="${index}" data-action="${item.action}" style="--x:${x}px;--y:${y}px;--delay:${index * 24}ms">${item.label}</button>`;
      }).join('');

      this.elements.submenuList.querySelectorAll('.submenu-item').forEach((item) => {
        item.addEventListener('mouseenter', () => {
          this.state.selectedSubmenuIndex = Number(item.dataset.idx);
          this.elements.submenuList.querySelectorAll('.submenu-item').forEach((entry, index) => entry.classList.toggle('is-active', index === this.state.selectedSubmenuIndex));
        });
        item.addEventListener('click', () => this.executeAction(item.dataset.action));
      });
    },

    handleKeyDown(event) {
      const now = Date.now();
      if (now - this.state.lastActionTime < 150) return;
      this.state.lastActionTime = now;

      if (event.key === 'ArrowRight' || event.key === 'ArrowLeft') {
        const direction = event.key === 'ArrowRight' ? 1 : -1;
        const index = (this.state.selectedSectorIndex + direction + this.sectors.length) % this.sectors.length;
        this.selectSector(index, this.state.submenuOpen);
        window.emitCEF('radial.navigate', event.key === 'ArrowRight' ? 'right' : 'left');
      } else if (event.key === 'ArrowDown' || event.key === 'ArrowUp') {
        const group = this.groups[this.state.activeGroup];
        if (!group) return;
        if (!this.state.submenuOpen) this.openSubmenu();
        const direction = event.key === 'ArrowDown' ? 1 : -1;
        if (group.items && group.items.length > 0) {
          this.state.selectedSubmenuIndex = (this.state.selectedSubmenuIndex + direction + group.items.length) % group.items.length;
          this.elements.submenuList.querySelectorAll('.submenu-item').forEach((item, index) => item.classList.toggle('is-active', index === this.state.selectedSubmenuIndex));
        }
        window.emitCEF('radial.navigate', event.key === 'ArrowDown' ? 'down' : 'up');
      } else if (event.key === 'Enter') {
        const group = this.groups[this.state.activeGroup];
        if (!this.state.submenuOpen) this.openSubmenu();
        else if (group && group.items && this.state.selectedSubmenuIndex >= 0 && group.items[this.state.selectedSubmenuIndex]) {
          this.executeAction(group.items[this.state.selectedSubmenuIndex].action);
        }
      } else if (event.key === 'Escape') {
        if (this.state.submenuOpen) this.closeSubmenu();
        else this.close();
      }
    },

    executeAction(action) {
      if (action === 'radial.documents.showDriverLicense') {
        window.emitCEF(action, this.state.targetId);
      } else {
        window.emitCEF('radial.select', action, this.state.targetId);
      }
      this.close();
    },

    close() {
      this.closeSubmenu();
      if (window.FocusManager) window.FocusManager.release('radialMenu');
      window.emitCEF('ui.radial.close');
    }
  };

  document.addEventListener('DOMContentLoaded', () => RadialMenu.init());
})();
