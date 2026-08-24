(function () {
  const MAX_SLOTS = 48;
  const TRADE_SLOTS = 30;
  const InventoryApp = {
    weight: 8.6,
    maxWeight: 15,
    tradeWeight: 0,
    tradeMaxWeight: 24,
    player: { skinId: 1, playerId: 27, name: 'HUB GTA' },
    tradeTargetId: null,
    items: [
      { slot: 0, id: 'sandwich', type: 'food', name: 'Sandwich', asset: 'images/sandwich.png', count: 2, weight: .4 },
      { slot: 1, id: 'water_bottle', type: 'water', name: 'Nước suối', asset: 'images/water_bottle.png', count: 4, weight: .5 },
      { slot: 2, id: 'weapon_pistol', type: 'weapon', name: 'Pistol .45', asset: 'images/weapon_pistol.png', count: 1, weight: 1.2 },
      { slot: 3, id: 'pistol_ammo', type: 'ammo', name: 'Đạn 9mm', asset: 'ammo_images/pistol_ammo.png', count: 30, weight: .02 },
      { slot: 4, id: 'firstaid', type: 'medical', name: 'Bộ sơ cứu', asset: 'images/firstaid.png', count: 2, weight: .8 },
      { slot: 5, id: 'driver_license', type: 'license', name: 'Giấy phép lái xe', asset: 'images/driver_license.png', count: 1, weight: .1 },
      { slot: 6, id: 'moneybag', type: 'currency', name: 'Tiền mặt', asset: 'images/moneybag.png', count: 18750, weight: 0 },
      { slot: 7, id: 'radio', type: 'radio', name: 'Radio LS', asset: 'images/radio.png', count: 1, weight: .6 },
      { slot: 8, id: 'labkey', type: 'key', name: 'Chìa khóa', asset: 'images/labkey.png', count: 2, weight: .1 }
    ],
    tradeItems: [],
    hotbar: [null, null, null, null, null, null],

    init() {
      this.cacheElements();
      this.bindEvents();
      this.setTradeActive(false);
      this.render();
      this.loadCatalog();
      if (window.FocusManager) window.FocusManager.request('inventoryApp', () => this.close());
    },

    cacheElements() {
      this.elements = {
        mainGrid: document.getElementById('mainGrid'),
        tradeGrid: document.getElementById('tradeGrid'),
        hotbarGrid: document.getElementById('hotbarGrid'),
        closeBtn: document.getElementById('closeInvBtn'),
        tradeBtn: document.getElementById('tradeBtn'),
        tradeCancelBtn: document.getElementById('tradeCancelBtn'),
        tradeOverlay: document.getElementById('tradeOverlay'),
        tradeClose: document.getElementById('tradeClose'),
        tradeCancel: document.getElementById('tradeCancel'),
        tradeConfirm: document.getElementById('tradeConfirm'),
        tradeTarget: document.getElementById('tradeTarget'),
        contextMenu: document.getElementById('contextMenu'),
        ctxItemTitle: document.getElementById('ctxItemTitle'),
        ctxItemMeta: document.getElementById('ctxItemMeta'),
        skin: document.getElementById('playerSkin'),
        skinStage: document.querySelector('.skin-stage')
      };
    },

    bindEvents() {
      this.elements.closeBtn.addEventListener('click', () => this.close());
      this.elements.tradeBtn.addEventListener('click', () => this.openTrade());
      this.elements.tradeCancelBtn.addEventListener('click', () => this.cancelTrade());
      this.elements.tradeClose.addEventListener('click', () => this.closeTrade());
      this.elements.tradeCancel.addEventListener('click', () => this.closeTrade());
      this.elements.tradeConfirm.addEventListener('click', () => {
        const targetId = Number(this.elements.tradeTarget.value);
        if (!Number.isFinite(targetId) || targetId < 0) return;
        this.tradeTargetId = targetId;
        document.getElementById('tradeTargetName').textContent = `Trade #${targetId}`;
        window.emitCEF('inventory.trade.request', targetId);
        this.setTradeActive(true);
        this.closeTrade();
      });
      this.elements.skin.addEventListener('load', () => this.elements.skinStage.classList.remove('is-error'));
      this.elements.skin.addEventListener('error', () => this.elements.skinStage.classList.add('is-error'));
      document.addEventListener('click', (event) => {
        if (this.elements.contextMenu && !this.elements.contextMenu.contains(event.target)) {
          this.hideContextMenu();
        }
      });
      document.addEventListener('keydown', (event) => { if (event.code === 'Escape') this.close(); });
      this.elements.contextMenu.addEventListener('click', (event) => {
        event.stopPropagation();
        const button = event.target.closest('[data-action]');
        if (!button || this.selectedSlot == null) return;
        const action = button.dataset.action;

        if (window.emitCEF) {
          window.emitCEF('inventory.action', this.selectedSlot, action);
        }

        const itemIndex = this.items.findIndex(i => i && Number(i.slot) === this.selectedSlot);
        if (itemIndex >= 0) {
          const item = this.items[itemIndex];
          if (action === 'use') {
            if (item.count > 1) item.count -= 1;
            else this.items.splice(itemIndex, 1);
          } else if (action === 'drop') {
            this.items.splice(itemIndex, 1);
          } else if (action === 'split') {
            if (item.count > 1) {
              const half = Math.floor(item.count / 2);
              item.count -= half;
              const emptySlot = Array.from({length: MAX_SLOTS}, (_, i) => i).find(s => !this.items.some(it => it && Number(it.slot) === s));
              if (emptySlot !== undefined) {
                this.items.push({ ...item, slot: emptySlot, count: half });
              }
            }
          }
        }

        this.hideContextMenu();
        this.render();
      });
      window.onCEF('inventory.trade.open', (targetId) => this.openTrade(targetId, true));
      window.onCEF('inventory.trade.setItems', (payload) => this.applyTradePayload(payload));
    },

    assetUrl(asset) {
      if (!asset) return 'assets/images/id_card.png';
      if (/^(https?:)?\/\//i.test(asset)) return asset;
      return `assets/${String(asset).replace(/^assets[\\/]/, '')}`;
    },

    listFor(container) { return container === 'trade' ? this.tradeItems : this.items; },

    itemFor(list, slot) { return list.find((item) => item && Number(item.slot) === slot); },

    slotMarkup(container, slot, item) {
      if (!item) return `<div class="slot-item empty" data-container="${container}" data-slot="${slot}"><span class="slot-index">${slot + 1}</span></div>`;
      const count = Number(item.count ?? item.quantity ?? 1).toLocaleString('vi-VN');
      return `<div class="slot-item" draggable="true" data-container="${container}" data-slot="${slot}"><span class="slot-index">${slot + 1}</span><span class="slot-icon"><img src="${this.assetUrl(item.asset)}" alt="${item.name || item.id}" loading="lazy"></span><span class="slot-name">${item.name || item.id}</span><span class="slot-count">${count}</span></div>`;
    },

    renderGrid(container, count) {
      const list = this.listFor(container);
      return Array.from({ length: count }, (_, slot) => this.slotMarkup(container, slot, this.itemFor(list, slot))).join('');
    },

    render() {
      this.elements.mainGrid.innerHTML = this.renderGrid('inventory', MAX_SLOTS);
      this.elements.tradeGrid.innerHTML = this.renderGrid('trade', TRADE_SLOTS);
      this.elements.hotbarGrid.innerHTML = '';
      document.getElementById('slotCounter').textContent = `${this.items.filter(Boolean).length} / ${MAX_SLOTS} ô`;
      document.getElementById('tradeSlotCounter').textContent = `${this.tradeItems.filter(Boolean).length} / ${TRADE_SLOTS} ô`;
      document.getElementById('weightLabel').textContent = `${Number(this.weight).toFixed(1)} / ${Number(this.maxWeight).toFixed(1)} KG`;
      document.getElementById('weightBar').style.width = `${Math.min(100, (this.weight / this.maxWeight) * 100)}%`;
      document.getElementById('tradeWeightLabel').textContent = `${Number(this.tradeWeight).toFixed(1)} / ${Number(this.tradeMaxWeight).toFixed(1)} KG`;
      document.getElementById('tradeWeightBar').style.width = `${Math.min(100, (this.tradeWeight / this.tradeMaxWeight) * 100)}%`;
      this.updatePlayer();
      this.bindSlotActions();
    },

    bindSlotActions() {
      [
        ['inventory', this.elements.mainGrid],
        ['trade', this.elements.tradeGrid]
      ].forEach(([container, grid]) => {
        grid.querySelectorAll('.slot-item').forEach((slot) => {
          if (container === 'inventory') {
            slot.addEventListener('click', (event) => {
              event.stopPropagation();
              if (slot.classList.contains('empty')) return;
              const rect = slot.getBoundingClientRect();
              this.openContextMenu(Number(slot.dataset.slot), rect.left + rect.width + 8, rect.top);
            });
            slot.addEventListener('contextmenu', (event) => {
              event.preventDefault();
              event.stopPropagation();
              if (slot.classList.contains('empty')) return;
              this.openContextMenu(Number(slot.dataset.slot), event.clientX, event.clientY);
            });
          }
          slot.addEventListener('dragstart', (event) => {
            if (slot.classList.contains('empty')) { event.preventDefault(); return; }
            const source = { container, slot: Number(slot.dataset.slot) };
            this.dragSource = source;
            event.dataTransfer.effectAllowed = 'move';
            event.dataTransfer.setData('text/plain', JSON.stringify(source));
            slot.classList.add('dragging');
            window.emitCEF('inventory.dragStart', container, source.slot);
          });
          slot.addEventListener('dragend', () => this.clearDragState());
          slot.addEventListener('dragover', (event) => { event.preventDefault(); event.dataTransfer.dropEffect = 'move'; slot.classList.add('drop-target'); });
          slot.addEventListener('dragleave', () => slot.classList.remove('drop-target'));
          slot.addEventListener('drop', (event) => {
            event.preventDefault();
            let source;
            try { source = JSON.parse(event.dataTransfer.getData('text/plain')); } catch (_) { source = this.dragSource; }
            if (source) this.moveItem(source.container, Number(source.slot), container, Number(slot.dataset.slot));
          });
        });
      });
    },

    moveItem(fromContainer, fromSlot, toContainer, toSlot) {
      if (fromContainer === toContainer && fromSlot === toSlot) return this.clearDragState();
      const fromList = this.listFor(fromContainer);
      const toList = this.listFor(toContainer);
      const fromIndex = fromList.findIndex((item) => item && Number(item.slot) === fromSlot);
      if (fromIndex < 0) return this.clearDragState();
      const targetIndex = toList.findIndex((item) => item && Number(item.slot) === toSlot);
      const moving = { ...fromList[fromIndex], slot: toSlot };
      if (fromList === toList) {
        const target = targetIndex >= 0 ? { ...toList[targetIndex], slot: fromSlot } : null;
        fromList[fromIndex] = moving;
        if (target && targetIndex !== fromIndex) fromList[targetIndex] = target;
      } else {
        const target = targetIndex >= 0 ? { ...toList[targetIndex], slot: fromSlot } : null;
        fromList.splice(fromIndex, 1);
        if (targetIndex >= 0) toList[targetIndex] = moving; else toList.push(moving);
        if (target) fromList.push(target);
        window.emitCEF('inventory.trade.move', fromContainer, toContainer, fromSlot, toSlot);
      }
      if (fromContainer === 'inventory' && toContainer === 'inventory') window.emitCEF('inventory.move', fromSlot, toSlot);
      this.clearDragState();
      this.render();
    },

    clearDragState() { document.querySelectorAll('.dragging, .drop-target').forEach((element) => element.classList.remove('dragging', 'drop-target')); this.dragSource = null; },

    openContextMenu(slot, x, y) {
      const item = this.itemFor(this.items, slot);
      if (!item) return;
      this.selectedSlot = slot;
      this.elements.ctxItemTitle.textContent = item.name || item.id;
      this.elements.ctxItemMeta.textContent = `${Number(item.count ?? item.quantity ?? 1).toLocaleString('vi-VN')} vật phẩm`;
      this.elements.contextMenu.style.left = `${Math.min(x, innerWidth - 202)}px`;
      this.elements.contextMenu.style.top = `${Math.min(y, innerHeight - 260)}px`;
      this.elements.contextMenu.classList.remove('hidden');
    },

    hideContextMenu() { this.elements.contextMenu.classList.add('hidden'); },
    setTradeActive(active) {
      this.isTradeActive = active;
      const externalBoard = document.querySelector('.external-board');
      if (externalBoard) {
        externalBoard.classList.toggle('hidden', !active);
      }
      this.elements.tradeBtn.classList.toggle('hidden', active);
      this.elements.tradeCancelBtn.classList.toggle('hidden', !active);
      const titleEl = document.getElementById('tradeTargetName');
      if (titleEl) {
        titleEl.textContent = active && this.tradeTargetId != null ? `Trade #${this.tradeTargetId}` : 'Kho giao dịch bên ngoài';
      }
    },
    openTrade(targetId, active = false) { if (Number.isFinite(Number(targetId))) this.elements.tradeTarget.value = Number(targetId); this.elements.tradeOverlay.classList.remove('hidden'); if (active) this.setTradeActive(true); },
    closeTrade() { this.elements.tradeOverlay.classList.add('hidden'); },
    cancelTrade() {
      const targetId = this.tradeTargetId;
      this.tradeTargetId = null;
      this.tradeItems = [];
      this.setTradeActive(false);
      this.render();
      window.emitCEF('inventory.trade.cancel', targetId);
    },

    updatePlayer() {
      const skinId = Number.isFinite(Number(this.player.skinId)) ? Number(this.player.skinId) : 1;
      const playerId = Number.isFinite(Number(this.player.playerId)) ? Number(this.player.playerId) : 27;
      const name = this.player.name || 'HUB GTA';
      this.elements.skin.src = `http://gtahub.kntech.co/skins/${skinId}.png`;
      document.getElementById('skinIdLabel').textContent = `SKIN ${skinId}`;
      document.getElementById('playerName').textContent = name;
      document.getElementById('playerCardName').textContent = name;
      document.getElementById('playerId').textContent = playerId;
    },

    applyTradePayload(payload) {
      if (Array.isArray(payload)) this.tradeItems = payload;
      else if (payload && typeof payload === 'object') {
        if (Array.isArray(payload.items)) this.tradeItems = payload.items;
        if (Number.isFinite(Number(payload.weight))) this.tradeWeight = Number(payload.weight);
        if (Number.isFinite(Number(payload.maxWeight))) this.tradeMaxWeight = Number(payload.maxWeight);
        if (payload.targetId !== undefined) this.tradeTargetId = payload.targetId;
      }
      this.render();
    },

    applyPayload(payload, weight, maxWeight, skinId, playerId, playerName) {
      if (payload && !Array.isArray(payload) && typeof payload === 'object') {
        if (Array.isArray(payload.items)) this.items = payload.items;
        if (Array.isArray(payload.hotbar)) this.hotbar = payload.hotbar;
        if (Array.isArray(payload.tradeItems)) this.tradeItems = payload.tradeItems;
        if (payload.wallet !== undefined) document.getElementById('walletValue').textContent = `$${Number(payload.wallet).toLocaleString('en-US')}`;
        if (payload.bank !== undefined) document.getElementById('bankValue').textContent = `$${Number(payload.bank).toLocaleString('en-US')}`;
        weight = payload.weight; maxWeight = payload.maxWeight; skinId = payload.skinId; playerId = payload.playerId; playerName = payload.playerName;
      } else if (Array.isArray(payload)) this.items = payload;
      if (Number.isFinite(Number(weight))) this.weight = Number(weight);
      if (Number.isFinite(Number(maxWeight))) this.maxWeight = Number(maxWeight);
      if (Number.isFinite(Number(skinId))) this.player.skinId = Number(skinId);
      if (Number.isFinite(Number(playerId))) this.player.playerId = Number(playerId);
      if (playerName) this.player.name = playerName;
      this.render();
    },

    async loadCatalog() {
      try {
        const response = await fetch('items.json', { cache: 'no-store' });
        if (!response.ok) throw new Error(`items.json ${response.status}`);
        const catalog = await response.json();
        const byId = new Map(catalog.map((item) => [item.id, item]));
        const hydrate = (item) => item ? { ...byId.get(item.id), ...item, asset: item.asset || byId.get(item.id)?.asset } : item;
        this.items = this.items.map(hydrate); this.tradeItems = this.tradeItems.map(hydrate); this.render();
      } catch (error) { console.warn('[Inventory] catalog fallback', error.message); }
    },

    close() { this.closeTrade(); this.setTradeActive(false); if (window.FocusManager) window.FocusManager.release('inventoryApp'); window.emitCEF('inventory.close'); }
  };

  window.InventoryUI = { setItems: (...args) => InventoryApp.applyPayload(...args), setSkin: (skinId, playerId, playerName) => { InventoryApp.player.skinId = skinId; InventoryApp.player.playerId = playerId ?? InventoryApp.player.playerId; InventoryApp.player.name = playerName || InventoryApp.player.name; InventoryApp.updatePlayer(); } };
  onCEF('inventory.setItems', (...args) => InventoryApp.applyPayload(...args));
  onCEF('inventory.show', (...args) => InventoryApp.applyPayload(...args));
  onCEF('inventory.setSkin', (skinId, playerId, playerName) => window.InventoryUI.setSkin(skinId, playerId, playerName));
  document.addEventListener('DOMContentLoaded', () => InventoryApp.init());
})();
