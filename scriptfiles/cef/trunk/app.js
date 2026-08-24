(function () {
  const MAX_POCKET_SLOTS = 48;
  const MAX_VEHICLE_SLOTS = 30;

  const TrunkApp = {
    selectedSlot: null,
    selectedContainer: null,
    dragSource: null,
    pocketWeight: 8.6,
    pocketMaxWeight: 15.0,
    vehicleWeight: 4.2,
    vehicleMaxWeight: 50.0,
    vehicleName: 'Vapid Dominator',
    vehiclePlate: 'HUB-27',

    playerItems: [
      { slot: 0, id: 'sandwich', name: 'Sandwich', asset: 'images/sandwich.png', count: 2, weight: 0.4 },
      { slot: 1, id: 'water_bottle', name: 'Nước suối', asset: 'images/water_bottle.png', count: 4, weight: 0.5 },
      { slot: 2, id: 'weapon_pistol', name: 'Pistol .45', asset: 'images/weapon_pistol.png', count: 1, weight: 1.2 },
      { slot: 3, id: 'pistol_ammo', name: 'Đạn 9mm', asset: 'ammo_images/pistol_ammo.png', count: 120, weight: 0.02 }
    ],
    vehicleItems: [
      { slot: 0, id: 'repairkit', name: 'Bộ sửa xe', asset: 'images/repairkit.png', count: 2, weight: 2.0 },
      { slot: 1, id: 'firstaid', name: 'Hộp y tế', asset: 'images/firstaid.png', count: 1, weight: 0.8 },
      { slot: 2, id: 'moneybag', name: 'Tiền mặt', asset: 'images/moneybag.png', count: 25000, weight: 0.0 }
    ],

    init() {
      this.cacheElements();
      this.bindEvents();
      this.render();
      this.loadCatalog();

      window.onCEF('trunk.setData', (payload) => this.applyPayload(payload));
    },

    cacheElements() {
      this.elements = {
        playerGrid: document.getElementById('playerInvGrid'),
        vehicleGrid: document.getElementById('vehicleTrunkGrid'),
        closeBtn: document.getElementById('closeTrunkBtn'),
        pocketWeightLabel: document.getElementById('pocketWeightLabel'),
        pocketWeightBar: document.getElementById('pocketWeightBar'),
        pocketSlotCounter: document.getElementById('pocketSlotCounter'),
        vehicleTitle: document.getElementById('vehicleTitle'),
        vehiclePlate: document.getElementById('vehiclePlate'),
        vehicleWeightLabel: document.getElementById('vehicleWeightLabel'),
        vehicleWeightBar: document.getElementById('vehicleWeightBar'),
        vehicleSlotCounter: document.getElementById('vehicleSlotCounter')
      };
    },

    bindEvents() {
      if (this.elements.closeBtn) {
        this.elements.closeBtn.addEventListener('click', () => this.close());
      }
      document.addEventListener('keydown', (event) => {
        if (event.code === 'Escape') this.close();
      });
    },

    assetUrl(asset) {
      if (!asset) return '../inventory/assets/images/id_card.png';
      if (/^(https?:)?\/\//i.test(asset)) return asset;
      return `../inventory/assets/${String(asset).replace(/^assets[\\/]/, '')}`;
    },

    slotMarkup(container, slot, item) {
      if (!item) {
        return `<div class="slot-item empty" data-container="${container}" data-slot="${slot}"><span class="slot-index">${slot + 1}</span></div>`;
      }
      const count = Number(item.count ?? item.quantity ?? 1).toLocaleString('vi-VN');
      const isSelected = this.selectedContainer === container && this.selectedSlot === slot;
      return `<div class="slot-item ${isSelected ? 'selected' : ''}" draggable="true" data-container="${container}" data-slot="${slot}"><span class="slot-index">${slot + 1}</span><span class="slot-icon"><img src="${this.assetUrl(item.asset)}" alt="${item.name || item.id}" loading="lazy"></span><span class="slot-name">${item.name || item.id}</span><span class="slot-count">${count}</span></div>`;
    },

    renderGrid(container, items, maxSlots) {
      const bySlot = new Map(items.map((item) => [Number(item.slot), item]));
      return Array.from({ length: maxSlots }, (_, slot) => this.slotMarkup(container, slot, bySlot.get(slot))).join('');
    },

    recalculateWeights() {
      const calcWeight = (items) => items.reduce((acc, item) => acc + (Number(item.weight || 0) * Number(item.count || 1)), 0);
      this.pocketWeight = calcWeight(this.playerItems);
      this.vehicleWeight = calcWeight(this.vehicleItems);
    },

    render() {
      this.recalculateWeights();

      this.elements.playerGrid.innerHTML = this.renderGrid('inventory', this.playerItems, MAX_POCKET_SLOTS);
      this.elements.vehicleGrid.innerHTML = this.renderGrid('trunk', this.vehicleItems, MAX_VEHICLE_SLOTS);

      this.elements.pocketWeightLabel.textContent = `${this.pocketWeight.toFixed(1)} / ${this.pocketMaxWeight.toFixed(1)} KG`;
      this.elements.pocketWeightBar.style.width = `${Math.min(100, (this.pocketWeight / this.pocketMaxWeight) * 100)}%`;
      this.elements.pocketSlotCounter.textContent = `${this.playerItems.filter(Boolean).length} / ${MAX_POCKET_SLOTS} ô`;

      this.elements.vehicleTitle.textContent = this.vehicleName;
      this.elements.vehiclePlate.textContent = `Khoang xe · ${this.vehiclePlate}`;
      this.elements.vehicleWeightLabel.textContent = `${this.vehicleWeight.toFixed(1)} / ${this.vehicleMaxWeight.toFixed(1)} KG`;
      this.elements.vehicleWeightBar.style.width = `${Math.min(100, (this.vehicleWeight / this.vehicleMaxWeight) * 100)}%`;
      this.elements.vehicleSlotCounter.textContent = `${this.vehicleItems.filter(Boolean).length} / ${MAX_VEHICLE_SLOTS} ô`;

      this.bindSlotActions();
    },

    bindSlotActions() {
      [
        ['inventory', this.elements.playerGrid],
        ['trunk', this.elements.vehicleGrid]
      ].forEach(([container, grid]) => {
        grid.querySelectorAll('.slot-item').forEach((slot) => {
          slot.addEventListener('click', () => this.selectSlot(slot));

          slot.addEventListener('dragstart', (event) => {
            if (slot.classList.contains('empty')) { event.preventDefault(); return; }
            const source = { container: slot.dataset.container, slot: Number(slot.dataset.slot) };
            this.dragSource = source;
            event.dataTransfer.effectAllowed = 'move';
            event.dataTransfer.setData('text/plain', JSON.stringify(source));
            slot.classList.add('dragging');
            window.emitCEF('inventory.trunk.dragStart', container, source.slot);
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

    selectSlot(element) {
      document.querySelectorAll('.slot-item').forEach((slot) => slot.classList.remove('selected'));
      element.classList.add('selected');
      this.selectedContainer = element.dataset.container;
      this.selectedSlot = Number(element.dataset.slot);
    },

    transferSelected(from, to) {
      if (this.selectedContainer !== from || this.selectedSlot === null) return;
      const targetItems = to === 'trunk' ? this.vehicleItems : this.playerItems;
      const maxSlots = to === 'trunk' ? MAX_VEHICLE_SLOTS : MAX_POCKET_SLOTS;
      const targetSlot = Array.from({ length: maxSlots }, (_, index) => index).find((slot) => !targetItems.some((item) => Number(item.slot) === slot));
      if (targetSlot === undefined) return;
      this.moveItem(from, this.selectedSlot, to, targetSlot);
    },

    moveItem(fromContainer, fromSlot, toContainer, toSlot) {
      if (fromContainer === toContainer && fromSlot === toSlot) return this.clearDragState();
      const fromItems = fromContainer === 'trunk' ? this.vehicleItems : this.playerItems;
      const toItems = toContainer === 'trunk' ? this.vehicleItems : this.playerItems;
      const fromIndex = fromItems.findIndex((item) => Number(item.slot) === fromSlot);
      if (fromIndex < 0) return this.clearDragState();

      const targetIndex = toItems.findIndex((item) => Number(item.slot) === toSlot);
      const moving = { ...fromItems[fromIndex], slot: toSlot };

      if (fromItems === toItems) {
        const target = targetIndex >= 0 ? { ...toItems[targetIndex], slot: fromSlot } : null;
        fromItems[fromIndex] = moving;
        if (target) fromItems[targetIndex] = target;
      } else {
        const target = targetIndex >= 0 ? { ...toItems[targetIndex], slot: fromSlot } : null;
        fromItems.splice(fromIndex, 1);
        if (targetIndex >= 0) toItems[targetIndex] = moving;
        else toItems.push(moving);
        if (target) fromItems.push(target);
      }

      const amount = moving.count || 1;
      window.emitCEF('inventory.trunkMove', fromContainer, toContainer, fromSlot, toSlot, amount);
      this.clearDragState();
      this.render();
    },

    clearDragState() {
      document.querySelectorAll('.dragging, .drop-target').forEach((element) => element.classList.remove('dragging', 'drop-target'));
      this.dragSource = null;
    },

    applyPayload(payload) {
      if (!payload) return;
      if (Array.isArray(payload.playerItems)) this.playerItems = payload.playerItems;
      if (Array.isArray(payload.vehicleItems)) this.vehicleItems = payload.vehicleItems;
      if (payload.vehicleName) this.vehicleName = payload.vehicleName;
      if (payload.vehiclePlate) this.vehiclePlate = payload.vehiclePlate;
      if (Number.isFinite(Number(payload.vehicleMaxWeight))) this.vehicleMaxWeight = Number(payload.vehicleMaxWeight);
      if (Number.isFinite(Number(payload.pocketMaxWeight))) this.pocketMaxWeight = Number(payload.pocketMaxWeight);
      this.render();
    },

    async loadCatalog() {
      try {
        const response = await fetch('../inventory/items.json', { cache: 'no-store' });
        if (!response.ok) return;
        const catalog = await response.json();
        const byId = new Map(catalog.map((item) => [item.id, item]));
        const hydrate = (item) => ({ ...byId.get(item.id), ...item, asset: item.asset || byId.get(item.id)?.asset });
        this.playerItems = this.playerItems.map(hydrate);
        this.vehicleItems = this.vehicleItems.map(hydrate);
        this.render();
      } catch (_) { /* local fallback */ }
    },

    close() {
      window.emitCEF('ui.trunk.close');
    }
  };

  document.addEventListener('DOMContentLoaded', () => TrunkApp.init());
})();
