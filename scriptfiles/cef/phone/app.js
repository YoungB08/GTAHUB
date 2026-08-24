document.addEventListener('DOMContentLoaded', () => {
  const state = {
    locked: true,
    currentApp: null,
    balance: 12450000,
    wallpaper: 'default',
    settings: {
      darkMode: true,
      vibration: true,
      notifications: true,
      volume: 72
    },
    activeCall: null,
    callStartedAt: 0,
    transactions: [
      { title: 'Los Santos Customs', detail: 'Nâng cấp phương tiện · 20:12', amount: -1200000, icon: '↗' },
      { title: 'Nguyễn An', detail: 'Nhận tiền · 18:44', amount: 500000, icon: '↓' },
      { title: '24/7 Market', detail: 'Thanh toán · Hôm qua', amount: -2750, icon: '↗' }
    ],
    bills: [
      { id: 1, title: 'Điện căn hộ', due: 'Hạn 12 tháng 8', amount: 3200, icon: 'ϟ', tone: 'power', paid: false },
      { id: 2, title: 'Nước sinh hoạt', due: 'Hạn 15 tháng 8', amount: 1550, icon: '≈', tone: 'water', paid: false },
      { id: 3, title: 'Bảo hiểm xe', due: 'Hạn 20 tháng 8', amount: 4000, icon: '◇', tone: 'vehicle', paid: false }
    ]
  };

  const contacts = [
    { name: 'Nguyễn An', id: '#15', initials: 'NA' },
    { name: 'Minh Trần', id: '#32', initials: 'MT' },
    { name: 'Gia Huy', id: '#08', initials: 'GH' },
    { name: 'Linh Phạm', id: '#41', initials: 'LP' },
    { name: 'Taxi Los Santos', id: '#911', initials: 'TX' }
  ];

  const phone = document.getElementById('phone');
  const phoneScreen = document.getElementById('phoneScreen');
  const lockScreen = document.getElementById('lockScreen');
  const homeScreen = document.getElementById('homeScreen');
  const appLayer = document.getElementById('appLayer');
  const appScreens = [...document.querySelectorAll('[data-screen]')];
  const dynamicIsland = document.getElementById('dynamicIsland');
  const callOverlay = document.getElementById('callOverlay');
  const toast = document.getElementById('toast');
  const balanceValue = document.getElementById('balanceValue');
  const transferBalance = document.getElementById('transferBalance');
  const transactionList = document.getElementById('transactionList');
  const billList = document.getElementById('billList');
  const money = new Intl.NumberFormat('en-US');
  let callTimer = null;
  let toastTimer = null;
  let pointerStartY = null;

  function emitClient(eventName, ...args) {
    if (window.cef && typeof window.cef.emit === 'function') {
      window.cef.emit(eventName, ...args);
    }
    window.dispatchEvent(new CustomEvent(eventName, { detail: args }));
  }

  function updatePhoneScale() {
    const viewportHeight = window.visualViewport?.height || window.innerHeight;
    const scale = Math.min(1, (viewportHeight * 0.92) / 1000);
    phone.style.setProperty('--phone-scale', scale.toFixed(4));
  }

  function updateClock() {
    const now = new Date();
    const time = now.toLocaleTimeString('vi-VN', { hour: '2-digit', minute: '2-digit', hour12: false });
    const longDate = new Intl.DateTimeFormat('vi-VN', { weekday: 'long', day: 'numeric', month: 'long' }).format(now);
    document.getElementById('statusTime').textContent = time;
    document.getElementById('lockTime').textContent = time;
    document.getElementById('lockDate').textContent = longDate.charAt(0).toUpperCase() + longDate.slice(1);
    document.getElementById('homeDate').textContent = longDate.charAt(0).toUpperCase() + longDate.slice(1);
  }

  function showToast(message) {
    clearTimeout(toastTimer);
    toast.textContent = message;
    toast.classList.add('is-visible');
    toastTimer = setTimeout(() => toast.classList.remove('is-visible'), 2200);
  }

  function unlockPhone() {
    if (!state.locked) return;
    state.locked = false;
    lockScreen.classList.add('is-unlocking');
    homeScreen.classList.add('is-active');
    setTimeout(() => {
      lockScreen.classList.remove('is-active', 'is-unlocking');
    }, 520);
    emitClient('phone:unlock');
  }

  function openApp(appName) {
    if (state.locked) return;
    const nextScreen = appScreens.find(screen => screen.dataset.screen === appName);
    if (!nextScreen) return;

    appScreens.forEach(screen => screen.classList.remove('is-active', 'is-closing'));
    state.currentApp = appName;
    homeScreen.classList.remove('is-active');
    appLayer.classList.add('is-open');
    phoneScreen.classList.add('app-open');
    nextScreen.classList.add('is-active');
    renderLiveData();
    emitClient('phone:openApp', appName);
  }

  function goHome() {
    if (state.locked || !state.currentApp) return;
    const currentScreen = appScreens.find(screen => screen.dataset.screen === state.currentApp);
    if (currentScreen) {
      currentScreen.classList.remove('is-active');
      currentScreen.classList.add('is-closing');
      setTimeout(() => currentScreen.classList.remove('is-closing'), 310);
    }
    state.currentApp = null;
    appLayer.classList.remove('is-open');
    phoneScreen.classList.remove('app-open');
    homeScreen.classList.add('is-active');
    emitClient('phone:home');
  }

  function renderContacts(query = '') {
    const normalized = query.trim().toLocaleLowerCase('vi-VN');
    const visible = contacts.filter(contact => `${contact.name} ${contact.id}`.toLocaleLowerCase('vi-VN').includes(normalized));
    document.getElementById('contactList').innerHTML = visible.map(contact => `
      <article class="contact-row">
        <div class="avatar">${contact.initials}</div>
        <div><strong>${contact.name}</strong><small>ID ${contact.id}</small></div>
        <button class="call-button" type="button" data-call-name="${contact.name}" data-call-avatar="${contact.initials}" aria-label="Gọi ${contact.name}">☎</button>
      </article>
    `).join('');
  }

  function renderTransactions() {
    const svgOut = `<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><line x1="7" y1="17" x2="17" y2="7"/><polyline points="7 7 17 7 17 17"/></svg>`;
    const svgIn = `<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><line x1="17" y1="7" x2="7" y2="17"/><polyline points="17 17 7 17 7 7"/></svg>`;
    transactionList.innerHTML = state.transactions.map(item => `
      <article class="transaction-row">
        <span class="transaction-icon">${item.amount >= 0 ? svgIn : svgOut}</span>
        <div><strong>${item.title}</strong><small>${item.detail}</small></div>
        <strong class="${item.amount >= 0 ? 'positive' : 'negative'}">${item.amount >= 0 ? '+' : '-'}$${money.format(Math.abs(item.amount))}</strong>
      </article>
    `).join('');
  }

  function renderBills() {
    const unpaidTotal = state.bills.filter(bill => !bill.paid).reduce((sum, bill) => sum + bill.amount, 0);
    document.getElementById('billTotal').textContent = `$${money.format(unpaidTotal)}`;
    billList.innerHTML = state.bills.map(bill => `
      <article class="bill-row">
        <span class="bill-icon ${bill.tone}">${bill.icon}</span>
        <div><strong>${bill.title}</strong><small>${bill.due} · $${money.format(bill.amount)}</small></div>
        <button class="bill-pay ${bill.paid ? 'is-paid' : ''}" type="button" data-bill-id="${bill.id}">${bill.paid ? 'Đã trả' : 'Thanh toán'}</button>
      </article>
    `).join('');
  }

  function renderLiveData() {
    const formattedBalance = `$${money.format(state.balance)}`;
    balanceValue.textContent = formattedBalance;
    transferBalance.textContent = formattedBalance;
    renderTransactions();
    renderBills();
  }

  function startCall(name, initials) {
    state.activeCall = { name, initials };
    state.callStartedAt = Date.now();
    document.getElementById('callName').textContent = name;
    document.getElementById('callAvatar').textContent = initials;
    document.getElementById('callStatus').textContent = 'đang gọi...';
    document.getElementById('islandCallText').textContent = name;
    callOverlay.classList.add('is-active');
    callOverlay.setAttribute('aria-hidden', 'false');
    phoneScreen.classList.add('calling');
    dynamicIsland.classList.add('has-call');
    clearInterval(callTimer);
    callTimer = setInterval(() => {
      const seconds = Math.floor((Date.now() - state.callStartedAt) / 1000);
      const minutes = String(Math.floor(seconds / 60)).padStart(2, '0');
      const remainder = String(seconds % 60).padStart(2, '0');
      document.getElementById('callStatus').textContent = `${minutes}:${remainder}`;
    }, 1000);
    emitClient('phone:call', name);
  }

  function endCall() {
    clearInterval(callTimer);
    callTimer = null;
    state.activeCall = null;
    callOverlay.classList.remove('is-active');
    callOverlay.setAttribute('aria-hidden', 'true');
    phoneScreen.classList.remove('calling');
    dynamicIsland.classList.remove('has-call');
    emitClient('phone:endCall');
  }

  function payBill(id) {
    const bill = state.bills.find(item => item.id === id);
    if (!bill || bill.paid) return;
    if (bill.amount > state.balance) {
      showToast('Số dư không đủ');
      return;
    }
    bill.paid = true;
    state.balance -= bill.amount;
    state.transactions.unshift({ title: bill.title, detail: 'Thanh toán hóa đơn · Vừa xong', amount: -bill.amount, icon: '↗' });
    renderLiveData();
    showToast('Thanh toán thành công');
    emitClient('phone:payBill', bill.id, bill.amount);
  }

  document.getElementById('unlockButton').addEventListener('click', unlockPhone);

  document.querySelectorAll('[data-app]').forEach(button => {
    button.addEventListener('click', () => openApp(button.dataset.app));
  });

  document.querySelectorAll('[data-home]').forEach(button => button.addEventListener('click', goHome));

  document.getElementById('contactSearch').addEventListener('input', event => renderContacts(event.target.value));

  document.getElementById('contactList').addEventListener('click', event => {
    const callButton = event.target.closest('[data-call-name]');
    if (callButton) startCall(callButton.dataset.callName, callButton.dataset.callAvatar);
  });

  document.getElementById('callClose').addEventListener('click', () => {
    callOverlay.classList.remove('is-active');
    callOverlay.setAttribute('aria-hidden', 'true');
    phoneScreen.classList.remove('calling');
  });

  document.getElementById('endCall').addEventListener('click', endCall);
  dynamicIsland.addEventListener('click', () => {
    if (state.activeCall) {
      callOverlay.classList.add('is-active');
      callOverlay.setAttribute('aria-hidden', 'false');
      phoneScreen.classList.add('calling');
    }
  });

  document.getElementById('muteButton').addEventListener('click', event => {
    const button = event.currentTarget;
    button.classList.toggle('is-muted');
    button.querySelector('span').textContent = button.classList.contains('is-muted') ? 'Bật tiếng' : 'Tắt tiếng';
  });

  document.getElementById('messageForm').addEventListener('submit', event => {
    event.preventDefault();
    const input = document.getElementById('messageInput');
    const message = input.value.trim();
    if (!message) return;
    const bubble = document.createElement('div');
    bubble.className = 'bubble outgoing';
    bubble.textContent = message;
    document.getElementById('messageThread').appendChild(bubble);
    input.value = '';
    bubble.scrollIntoView({ behavior: 'smooth', block: 'end' });
    emitClient('phone:message', message);
  });

  document.querySelectorAll('[data-amount]').forEach(button => {
    button.addEventListener('click', () => {
      document.getElementById('transferAmount').value = button.dataset.amount;
    });
  });

  document.getElementById('transferForm').addEventListener('submit', event => {
    event.preventDefault();
    const receiver = Number(document.getElementById('receiverId').value);
    const amount = Math.floor(Number(document.getElementById('transferAmount').value));
    const note = document.getElementById('transferNote').value.trim() || 'Chuyển khoản';
    const error = document.getElementById('transferError');

    if (!Number.isInteger(receiver) || receiver <= 0) {
      error.textContent = 'Vui lòng nhập ID người nhận hợp lệ.';
      return;
    }
    if (!Number.isFinite(amount) || amount < 1) {
      error.textContent = 'Số tiền phải lớn hơn $0.';
      return;
    }
    if (amount > state.balance) {
      error.textContent = 'Số dư tài khoản không đủ.';
      return;
    }

    error.textContent = '';
    state.balance -= amount;
    state.transactions.unshift({ title: `Người chơi #${receiver}`, detail: `${note} · Vừa xong`, amount: -amount, icon: '↗' });
    event.currentTarget.reset();
    renderLiveData();
    showToast(`Đã chuyển $${money.format(amount)} tới #${receiver}`);
    emitClient('phone:transfer', receiver, amount, note);
  });

  billList.addEventListener('click', event => {
    const payButton = event.target.closest('[data-bill-id]');
    if (payButton) payBill(Number(payButton.dataset.billId));
  });

  const settingsBindings = [
    ['notificationsToggle', 'notifications'],
    ['vibrationToggle', 'vibration'],
    ['darkModeToggle', 'darkMode']
  ];

  settingsBindings.forEach(([elementId, key]) => {
    document.getElementById(elementId).addEventListener('change', event => {
      state.settings[key] = event.target.checked;
      if (key === 'darkMode') phoneScreen.classList.toggle('dark-settings', event.target.checked);
      emitClient('phone:setting', key, state.settings[key]);
    });
  });

  document.getElementById('volumeRange').addEventListener('input', event => {
    state.settings.volume = Number(event.target.value);
    emitClient('phone:setting', 'volume', state.settings.volume);
  });

  document.getElementById('musicButton').addEventListener('click', () => showToast('Không có bài hát đang phát'));

  document.getElementById('exitApp').addEventListener('click', () => {
    phone.classList.add('is-hidden');
    emitClient('phone:close');
  });

  phoneScreen.addEventListener('pointerdown', event => {
    pointerStartY = event.clientY;
  });

  phoneScreen.addEventListener('pointerup', event => {
    if (pointerStartY === null) return;
    const distance = event.clientY - pointerStartY;
    pointerStartY = null;
    if (state.locked && distance < -65) unlockPhone();
    if (!state.locked && state.currentApp && distance > 85) goHome();
  });

  document.addEventListener('keydown', event => {
    if (event.key === 'Escape') {
      if (state.activeCall && callOverlay.classList.contains('is-active')) {
        callOverlay.classList.remove('is-active');
      } else if (state.currentApp) {
        goHome();
      }
    }
  });

  window.addEventListener('resize', updatePhoneScale);
  window.visualViewport?.addEventListener('resize', updatePhoneScale);

  window.gtaPhone = {
    show() { phone.classList.remove('is-hidden'); },
    hide() { phone.classList.add('is-hidden'); },
    lock() {
      state.locked = true;
      state.currentApp = null;
      appScreens.forEach(screen => screen.classList.remove('is-active', 'is-closing'));
      appLayer.classList.remove('is-open');
      phoneScreen.classList.remove('app-open');
      homeScreen.classList.remove('is-active');
      lockScreen.classList.add('is-active');
    },
    setBalance(value) {
      if (Number.isFinite(Number(value))) {
        state.balance = Number(value);
        renderLiveData();
      }
    },
    openApp
  };

  phoneScreen.classList.toggle('dark-settings', state.settings.darkMode);
  updatePhoneScale();
  updateClock();
  setInterval(updateClock, 1000);
  renderContacts();
  renderLiveData();
});
