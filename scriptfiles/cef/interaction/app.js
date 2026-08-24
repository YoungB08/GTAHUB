(function () {
  const InteractionHUD = {
    cardEl: null,
    init() {
      this.cardEl = document.getElementById('hintCard');
      window.onCEF('interaction.showHint', (key, title, subtitle, context) => this.show(key, title, subtitle, context));
      window.onCEF('interaction.hideHint', () => this.hide());
      document.getElementById('acceptBtn').addEventListener('click', () => window.emitCEF('interaction.response', true));
      document.getElementById('cancelBtn').addEventListener('click', () => window.emitCEF('interaction.response', false));
    },
    show(key = 'E', title = 'Tương tác', subtitle = 'Nhấn phím để thực hiện', context = 'TƯƠNG TÁC GẦN') {
      document.getElementById('hintKey').textContent = key;
      document.getElementById('hintTitle').textContent = title;
      document.getElementById('hintText').textContent = subtitle;
      document.getElementById('hintContext').textContent = context;
      const progress = this.cardEl.querySelector('.hint-progress span');
      progress.style.animation = 'none';
      void progress.offsetWidth;
      progress.style.animation = 'interaction-timer 4s linear forwards';
      this.cardEl.classList.remove('hidden');
    },
    hide() { this.cardEl.classList.add('hidden'); }
  };
  document.addEventListener('DOMContentLoaded', () => InteractionHUD.init());
})();
