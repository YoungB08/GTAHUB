(function () {
  /**
   * UIFocusManager — Manages z-index focus and ESC key stack for SA:MP CEF UI screens
   */
  class UIFocusManager {
    constructor() {
      this.stack = [];
      this.baseZIndex = 1000;
      this.bindEvents();
    }

    bindEvents() {
      window.addEventListener('keydown', (e) => {
        if (e.key === 'Escape' || e.keyCode === 27) {
          this.handleEsc();
        }
      });
    }

    request(screenId, closeCallback) {
      this.release(screenId); // Prevent duplicate push
      const zIndex = this.baseZIndex + (this.stack.length + 1) * 10;
      const entry = { id: screenId, closeCallback, zIndex };
      this.stack.push(entry);

      window.emitCEF('ui.focus.request', screenId);
      return zIndex;
    }

    release(screenId) {
      const index = this.stack.findIndex((item) => item.id === screenId);
      if (index !== -1) {
        this.stack.splice(index, 1);
        window.emitCEF('ui.focus.release', screenId);
      }
    }

    handleEsc() {
      if (this.stack.length === 0) return;
      const topMost = this.stack[this.stack.length - 1];
      if (topMost && typeof topMost.closeCallback === 'function') {
        topMost.closeCallback();
      }
    }

    getTopMost() {
      return this.stack.length > 0 ? this.stack[this.stack.length - 1] : null;
    }
  }

  window.FocusManager = new UIFocusManager();
})();
