(function () {
  /**
   * ScreenManager — iOS-inspired navigation stack router for SA:MP CEF UI
   */
  class ScreenManager {
    constructor() {
      this.screens = new Map();
      this.historyStack = [];
      this.activeScreen = null;
    }

    register(name, screenInstance) {
      this.screens.set(name, screenInstance);
    }

    open(screenName, data = {}) {
      const screen = this.screens.get(screenName);
      if (!screen) {
        console.warn(`[ScreenManager] Screen not found: ${screenName}`);
        return;
      }

      if (this.activeScreen && typeof this.activeScreen.pause === 'function') {
        this.activeScreen.pause();
      }

      this.historyStack = [screenName];
      this.activeScreen = screen;

      if (typeof screen.mount === 'function') {
        screen.mount(data);
      }
      if (typeof screen.resume === 'function') {
        screen.resume();
      }

      window.emitCEF('ui.screen.change', { screen: screenName, action: 'open' });
    }

    close(screenName) {
      const targetName = screenName || (this.activeScreen ? this.activeScreen.name : null);
      if (!targetName) return;

      const screen = this.screens.get(targetName);
      if (screen) {
        if (typeof screen.pause === 'function') screen.pause();
        if (typeof screen.unmount === 'function') screen.unmount();
        if (typeof screen.destroy === 'function') screen.destroy();
      }

      this.historyStack = this.historyStack.filter((s) => s !== targetName);
      if (this.activeScreen && this.activeScreen.name === targetName) {
        this.activeScreen = null;
        if (this.historyStack.length > 0) {
          const prevName = this.historyStack[this.historyStack.length - 1];
          const prevScreen = this.screens.get(prevName);
          if (prevScreen) {
            this.activeScreen = prevScreen;
            if (typeof prevScreen.resume === 'function') prevScreen.resume();
          }
        }
      }

      window.emitCEF('ui.screen.change', { screen: targetName, action: 'close' });
    }

    push(screenName, data = {}) {
      const screen = this.screens.get(screenName);
      if (!screen) return;

      if (this.activeScreen && typeof this.activeScreen.pause === 'function') {
        this.activeScreen.pause();
      }

      this.historyStack.push(screenName);
      this.activeScreen = screen;

      if (typeof screen.mount === 'function') {
        screen.mount(data);
      }
      if (typeof screen.resume === 'function') {
        screen.resume();
      }
    }

    pop() {
      if (this.historyStack.length <= 1) {
        this.close();
        return;
      }

      const poppedName = this.historyStack.pop();
      const poppedScreen = this.screens.get(poppedName);
      if (poppedScreen) {
        if (typeof poppedScreen.pause === 'function') poppedScreen.pause();
        if (typeof poppedScreen.unmount === 'function') poppedScreen.unmount();
      }

      const currentName = this.historyStack[this.historyStack.length - 1];
      const currentScreen = this.screens.get(currentName);
      if (currentScreen) {
        this.activeScreen = currentScreen;
        if (typeof currentScreen.resume === 'function') currentScreen.resume();
      }
    }

    replace(screenName, data = {}) {
      this.pop();
      this.push(screenName, data);
    }
  }

  window.Router = new ScreenManager();
})();
