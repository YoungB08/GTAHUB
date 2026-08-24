(function () {
  /**
   * Mobile Banking Controller
   */
  const BankingApp = {
    state: {
      cash: 5000,
      bank: 24750000,
      isSubmitting: false,
      activeAction: null,
      history: [
        { id: 'TX-1021', title: 'Nhận lương PD', amount: 5700, type: 'plus', date: '08/08 18:44', note: 'Lương tuần LS Police' },
        { id: 'TX-1020', title: 'LS Customs', amount: -1200, type: 'minus', date: '08/08 16:30', note: 'Sửa xe Vapid' },
        { id: 'TX-1019', title: 'Nguyễn An', amount: 500, type: 'plus', date: '08/08 14:15', note: 'Trả nợ tiền nước' }
      ]
    },

    init() {
      this.cacheElements();
      this.bindEvents();
      this.renderHistory();

      window.onCEF('bank.updateBalance', (cash, bank) => this.updateBalance(cash, bank));
      window.onCEF('bank.setHistory', (history) => {
        if (Array.isArray(history)) { this.state.history = history; this.renderHistory(); }
      });

      // Initial history request
      window.emitCEF('bank.requestHistory');
    },

    cacheElements() {
      this.elements = {
        totalBalance: document.getElementById('totalBalance'),
        txList: document.getElementById('txList'),
        transferBtn: document.getElementById('transferBtn'),
        depositBtn: document.getElementById('depositBtn'),
        withdrawBtn: document.getElementById('withdrawBtn'),
        payBillBtn: document.getElementById('payBillBtn'),
        bankingModal: document.getElementById('bankingModal'),
        modalTitle: document.getElementById('modalTitle'),
        modalClose: document.getElementById('modalClose'),
        bankingForm: document.getElementById('bankingForm'),
        txTarget: document.getElementById('txTarget'),
        txAmount: document.getElementById('txAmount'),
        txNote: document.getElementById('txNote'),
        targetField: document.getElementById('targetField'),
        noteField: document.getElementById('noteField'),
        formSubmitBtn: document.getElementById('formSubmitBtn'),
        submitSpinner: document.getElementById('submitSpinner'),
        submitLabel: document.getElementById('submitLabel'),
        txDetailSheet: document.getElementById('txDetailSheet'),
        sheetClose: document.getElementById('sheetClose'),
      };
    },

    bindEvents() {
      this.elements.transferBtn.addEventListener('click', () => this.openModal('transfer'));
      this.elements.depositBtn.addEventListener('click', () => this.openModal('deposit'));
      this.elements.withdrawBtn.addEventListener('click', () => this.openModal('withdraw'));
      this.elements.payBillBtn.addEventListener('click', () => this.openModal('bill'));
      this.elements.modalClose.addEventListener('click', () => this.closeModal());
      this.elements.sheetClose.addEventListener('click', () => this.elements.txDetailSheet.classList.add('hidden'));

      document.querySelectorAll('.bank-nav-item').forEach((navBtn) => {
        navBtn.addEventListener('click', () => {
          document.querySelectorAll('.bank-nav-item').forEach((btn) => btn.classList.remove('is-active'));
          navBtn.classList.add('is-active');

          const view = navBtn.dataset.bankView;
          if (view === 'transfer') this.openModal('transfer');
          else if (view === 'deposit') this.openModal('deposit');
          else if (view === 'withdraw') this.openModal('withdraw');
          else if (view === 'bills') this.openModal('bill');
          else if (view === 'overview') this.closeModal();
        });
      });

      this.elements.bankingForm.addEventListener('submit', (e) => {
        e.preventDefault();
        this.handleSubmit();
      });
    },

    updateBalance(cash, bank) {
      this.state.cash = cash;
      this.state.bank = bank;
      const formatted = new Intl.NumberFormat('en-US', { style: 'currency', currency: 'USD', maximumFractionDigits: 0 }).format(bank);
      this.elements.totalBalance.textContent = formatted;
    },

    renderHistory() {
      const plusSvg = `<svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#22c55e" stroke-width="2.5"><circle cx="12" cy="12" r="10" fill="rgba(34,197,94,0.12)"/><polyline points="16 12 12 16 8 12"/><line x1="12" y1="8" x2="12" y2="16"/></svg>`;
      const minusSvg = `<svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#ef4444" stroke-width="2.5"><circle cx="12" cy="12" r="10" fill="rgba(239,68,68,0.12)"/><polyline points="8 12 12 8 16 12"/><line x1="12" y1="16" x2="12" y2="8"/></svg>`;

      this.elements.txList.innerHTML = this.state.history.map((tx) => `
        <div class="tx-item" data-id="${tx.id}">
          <div class="tx-icon">${tx.type === 'plus' ? plusSvg : minusSvg}</div>
          <div class="tx-info">
            <strong>${tx.title}</strong>
            <small>${tx.date}</small>
          </div>
          <div class="tx-amount ${tx.type === 'plus' ? 'text-success' : 'text-danger'}">
            ${tx.type === 'plus' ? '+' : '-'}$${Math.abs(tx.amount).toLocaleString()}
          </div>
        </div>
      `).join('');

      document.querySelectorAll('.tx-item').forEach((item) => {
        item.addEventListener('click', () => {
          const id = item.dataset.id;
          const tx = this.state.history.find((t) => t.id === id);
          if (tx) this.openDetailSheet(tx);
        });
      });
    },

    openDetailSheet(tx) {
      document.getElementById('sheetTxId').textContent = tx.id;
      document.getElementById('sheetTarget').textContent = tx.title;
      document.getElementById('sheetAmount').textContent = `${tx.type === 'plus' ? '+' : '-'}$${Math.abs(tx.amount).toLocaleString()}`;
      document.getElementById('sheetTime').textContent = tx.date;
      document.getElementById('sheetNote').textContent = tx.note || 'Không có ghi chú';
      this.elements.txDetailSheet.classList.remove('hidden');
    },

    openModal(action) {
      this.state.activeAction = action;
      this.elements.bankingForm.reset();
      this.elements.bankingModal.classList.remove('hidden');

      if (action === 'transfer') {
        this.elements.modalTitle.textContent = 'Chuyển khoản';
        this.elements.targetField.classList.remove('hidden');
        this.elements.noteField.classList.remove('hidden');
      } else if (action === 'deposit') {
        this.elements.modalTitle.textContent = 'Nạp tiền vào tài khoản';
        this.elements.targetField.classList.add('hidden');
        this.elements.noteField.classList.add('hidden');
      } else if (action === 'withdraw') {
        this.elements.modalTitle.textContent = 'Rút tiền mặt';
        this.elements.targetField.classList.add('hidden');
        this.elements.noteField.classList.add('hidden');
      } else if (action === 'bill') {
        this.elements.modalTitle.textContent = 'Thanh toán hóa đơn';
        this.elements.targetField.classList.remove('hidden');
        this.elements.noteField.classList.add('hidden');
      }
    },

    closeModal() {
      this.elements.bankingModal.classList.add('hidden');
      this.state.activeAction = null;
    },

    handleSubmit() {
      if (this.state.isSubmitting) return;

      const action = this.state.activeAction;
      const targetId = parseInt(this.elements.txTarget.value, 10);
      const amount = parseInt(this.elements.txAmount.value, 10);
      const note = this.elements.txNote.value;

      if (!amount || amount <= 0) return;

      this.setSubmitting(true);

      if (action === 'transfer') {
        window.emitCEF('bank.transfer', targetId, amount, note);
      } else if (action === 'deposit') {
        window.emitCEF('bank.deposit', amount);
      } else if (action === 'withdraw') {
        window.emitCEF('bank.withdraw', amount);
      } else if (action === 'bill') {
        window.emitCEF('bank.payBill', targetId);
      }

      setTimeout(() => {
        this.setSubmitting(false);
        this.closeModal();
      }, 600);
    },

    setSubmitting(val) {
      this.state.isSubmitting = val;
      if (val) {
        this.elements.submitSpinner.classList.remove('hidden');
        this.elements.formSubmitBtn.disabled = true;
      } else {
        this.elements.submitSpinner.classList.add('hidden');
        this.elements.formSubmitBtn.disabled = false;
      }
    }
  };

  document.addEventListener('DOMContentLoaded', () => BankingApp.init());
})();
