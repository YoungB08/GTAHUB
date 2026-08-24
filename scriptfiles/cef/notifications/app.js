(function () {
  /**
   * Priority Toast Notification Controller
   */
  const ToastSystem = {
    stackEl: null,
    queue: [],
    maxVisible: 4,
    recentSpamMap: new Map(),

    icons: {
      success: `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polyline points="20 6 9 17 4 12"/></svg>`,
      error: `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/></svg>`,
      warning: `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>`,
      info: `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><line x1="12" y1="16" x2="12" y2="12"/><line x1="12" y1="8" x2="12.01" y2="8"/></svg>`
    },

    init() {
      this.stackEl = document.getElementById('toastStack');
      window.onCEF('notify.show', (type, title, message, duration, priority) => {
        this.push(type, title, message, duration, priority);
      });

      // Initial Demo Notifications
      this.push('success', 'Nhận lương PD', 'Bạn đã nhận +$5,700 vào tài khoản', 4000, 'normal');
      setTimeout(() => {
        this.push('warning', 'Khóa tay Cảnh sát', 'Bạn đã bị cảnh sát khóa tay!', 5000, 'high');
      }, 800);
    },

    push(type = 'info', title = '', message = '', duration = 4000, priority = 'normal') {
      const spamKey = `${type}:${title}:${message}`;
      const now = Date.now();

      // Anti-spam deduplication check (2000ms window)
      if (this.recentSpamMap.has(spamKey)) {
        const lastTime = this.recentSpamMap.get(spamKey);
        if (now - lastTime < 2000) return;
      }
      this.recentSpamMap.set(spamKey, now);

      const toastData = { type, title, message, duration, priority, id: now };

      if (priority === 'high') {
        this.queue.unshift(toastData); // High priority goes to front
      } else {
        this.queue.push(toastData);
      }

      this.processQueue();
    },

    processQueue() {
      while (this.queue.length > 0 && this.stackEl.children.length < this.maxVisible) {
        const item = this.queue.shift();
        this.renderToast(item);
      }
    },

    renderToast(item) {
      const toast = document.createElement('article');
      toast.className = `toast toast-slide ${item.type} priority-${item.priority || 'normal'}`;
      toast.innerHTML = `
        <div class="toast-icon">${this.icons[item.type] || this.icons.info}</div>
        <div class="toast-content">
          <h4 class="toast-title"></h4>
          <p class="toast-message"></p>
        </div>
        <button class="toast-close" type="button"><svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><line x1="18" y1="6" x2="6" y2="18"/><line x1="6" y1="6" x2="18" y2="18"/></svg></button>
        <div class="toast-progress" style="animation-duration: ${item.duration}ms;"></div>
      `;

      toast.querySelector('.toast-title').textContent = item.title || 'Thông báo';
      toast.querySelector('.toast-message').textContent = item.message || '';

      const dismiss = () => {
        toast.style.opacity = '0';
        toast.style.transform = 'translateX(50px)';
        toast.style.transition = 'all 0.25s ease';
        setTimeout(() => {
          toast.remove();
          this.processQueue();
        }, 250);
      };

      toast.querySelector('.toast-close').addEventListener('click', dismiss);
      setTimeout(dismiss, item.duration || 4000);

      this.stackEl.appendChild(toast);
    }
  };

  document.addEventListener('DOMContentLoaded', () => ToastSystem.init());
})();
