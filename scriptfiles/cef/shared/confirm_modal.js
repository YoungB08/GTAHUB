(function () {
  /**
   * ConfirmModal — Shared confirmation modal component
   */
  class ConfirmModalComponent {
    constructor() {
      this.modalEl = null;
      this.currentId = null;
      this.initDOM();
      this.bindEvents();
    }

    initDOM() {
      const container = document.createElement('div');
      container.id = 'sharedConfirmModal';
      container.className = 'confirm-modal-overlay hidden';
      container.innerHTML = `
        <div class="confirm-modal-card scale-open">
          <div class="confirm-modal-header">
            <span class="confirm-modal-icon"><svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#f59e0b" stroke-width="2.5"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg></span>
            <h3 id="sharedConfirmTitle">Xác nhận thao tác</h3>
          </div>
          <p id="sharedConfirmMessage">Bạn có chắc chắn muốn thực hiện hành động này?</p>
          <div class="confirm-modal-actions">
            <button id="sharedConfirmCancel" class="btn btn-secondary" type="button">Hủy bỏ</button>
            <button id="sharedConfirmOk" class="btn btn-danger" type="button">Xác nhận</button>
          </div>
        </div>
      `;
      document.body.appendChild(container);
      this.modalEl = container;
    }

    bindEvents() {
      const cancelBtn = this.modalEl.querySelector('#sharedConfirmCancel');
      const okBtn = this.modalEl.querySelector('#sharedConfirmOk');

      cancelBtn.addEventListener('click', () => this.respond(false));
      okBtn.addEventListener('click', () => this.respond(true));

      window.onCEF('ui.modal.confirm', (id, title, message) => {
        this.show(id, title, message);
      });
    }

    show(id, title, message) {
      this.currentId = id;
      this.modalEl.querySelector('#sharedConfirmTitle').textContent = title || 'Xác nhận';
      this.modalEl.querySelector('#sharedConfirmMessage').textContent = message || '';
      this.modalEl.classList.remove('hidden');

      if (window.FocusManager) {
        window.FocusManager.request('confirmModal', () => this.respond(false));
      }
    }

    respond(confirmed) {
      if (this.currentId) {
        window.emitCEF('ui.modal.confirmResult', this.currentId, confirmed);
      }
      this.modalEl.classList.add('hidden');
      if (window.FocusManager) {
        window.FocusManager.release('confirmModal');
      }
      this.currentId = null;
    }
  }

  document.addEventListener('DOMContentLoaded', () => {
    window.ConfirmModal = new ConfirmModalComponent();
  });
})();
