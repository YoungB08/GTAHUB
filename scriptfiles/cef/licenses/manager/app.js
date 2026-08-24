(function () {
  /**
   * License Manager & QR Verify Controller
   */
  const LicenseManager = {
    init() {
      this.cacheElements();
      this.bindEvents();

      window.onCEF('license.verifyResult', (status, owner, expireDate) => {
        this.updateVerifyStatus(status, owner, expireDate);
      });
    },

    cacheElements() {
      this.elements = {
        qrVerifyBtn: document.getElementById('qrVerifyBtn'),
        qrModal: document.getElementById('qrModal'),
        qrCloseBtn: document.getElementById('qrCloseBtn'),
        verifyStatus: document.getElementById('verifyStatus'),
      };
    },

    bindEvents() {
      if (this.elements.qrVerifyBtn) {
        this.elements.qrVerifyBtn.addEventListener('click', () => {
          this.elements.qrModal.classList.remove('hidden');
          window.emitCEF('license.verify', 'SA-LIC-99218');
        });
      }

      if (this.elements.qrCloseBtn) {
        this.elements.qrCloseBtn.addEventListener('click', () => {
          this.elements.qrModal.classList.add('hidden');
        });
      }

      document.querySelectorAll('.register-btn').forEach((btn) => {
        btn.addEventListener('click', () => {
          const type = btn.dataset.type;
          window.emitCEF('license.registerTest', type);
        });
      });
    },

    updateVerifyStatus(status, owner, expireDate) {
      this.elements.verifyStatus.innerHTML = `
        <span>Sở hữu: <b>${owner || 'HUB GTA'}</b></span><br>
        <span>Hạn dùng: <b>${expireDate || '08/08/2027'}</b></span>
        <p class="${status ? 'text-success' : 'text-danger'}">${status ? '✔ Giấy phép hợp lệ' : '✖ Giấy phép hết hạn'}</p>
      `;
    }
  };

  document.addEventListener('DOMContentLoaded', () => LicenseManager.init());
})();
