(function () {
  const handlerMap = new Map();

  /**
   * Emit an event from CEF JS to SA:MP Client / Pawn.
   * @param {string} event 
   * @param  {...any} args 
   */
  window.emitCEF = function (event, ...args) {
    if (typeof cef !== 'undefined' && cef && typeof cef.emit === 'function') {
      cef.emit(event, ...args);
    } else {
      console.log('[SAMP CEF EMIT]', event, args);
      window.dispatchEvent(new CustomEvent('cef:emit', { detail: { event, args } }));
    }
  };

  /**
   * Register an event listener for SA:MP -> CEF JS.
   * @param {string} event 
   * @param {Function} handler 
   */
  window.onCEF = function (event, handler) {
    if (!handlerMap.has(event)) {
      handlerMap.set(event, new Set());
    }
    handlerMap.get(event).add(handler);

    if (typeof cef !== 'undefined' && cef && typeof cef.on === 'function') {
      cef.on(event, handler);
    } else {
      console.log('[SAMP CEF ON]', event);
    }
  };

  /**
   * Unregister an event listener in JS space.
   * @param {string} event 
   * @param {Function} handler 
   */
  window.offCEF = function (event, handler) {
    if (handlerMap.has(event)) {
      handlerMap.get(event).delete(handler);
    }
    if (typeof cef !== 'undefined' && cef && typeof cef.off === 'function') {
      cef.off(event, handler);
    } else {
      console.log('[SAMP CEF OFF]', event);
    }
  };

  /**
   * Mock event emission for browser testing/debugging.
   * @param {string} event 
   * @param  {...any} args 
   */
  window.mockCEF = function (event, ...args) {
    const handlers = handlerMap.get(event);
    if (handlers) {
      handlers.forEach((fn) => fn(...args));
    }
  };
})();
