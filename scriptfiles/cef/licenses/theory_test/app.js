(function () {
  const QUESTION_BANKS = {
    car: [
      { q: 'Khi gặp đèn đỏ, người lái xe phải làm gì?', a: ['Tăng tốc vượt qua', 'Dừng trước vạch dừng', 'Bấm còi liên tục', 'Rẽ phải ngay lập tức'], correct: 1 },
      { q: 'Biển báo hình tam giác viền đỏ thường biểu thị?', a: ['Chỉ dẫn', 'Nguy hiểm / cảnh báo', 'Cấm dừng', 'Khu dân cư'], correct: 1 },
      { q: 'Bạn được phép đỗ xe ở đâu?', a: ['Giữa giao lộ', 'Nơi có biển cấm', 'Khu vực được đánh dấu đỗ xe', 'Trên vạch qua đường'], correct: 2 },
      { q: 'Khoảng cách an toàn giúp người lái xe làm gì?', a: ['Có thời gian xử lý tình huống', 'Đi nhanh hơn', 'Bật đèn cảnh báo', 'Không cần quan sát'], correct: 0 },
      { q: 'Khi chuyển làn, thao tác nào là bắt buộc?', a: ['Bật xi nhan và quan sát', 'Tắt đèn xe', 'Bấm còi liên tục', 'Dừng giữa làn'], correct: 0 }
    ],
    truck: [
      { q: 'Xe tải chở hàng phải ưu tiên kiểm tra điều gì trước khi chạy?', a: ['Hàng được chằng buộc chắc chắn', 'Mở cửa thùng xe', 'Tắt đèn hậu', 'Tăng tốc tối đa'], correct: 0 },
      { q: 'Khi xuống dốc dài bằng xe tải, nên làm gì?', a: ['Về số thấp và kiểm soát tốc độ', 'Tắt máy', 'Về số N', 'Đạp ga liên tục'], correct: 0 },
      { q: 'Điểm mù của xe tải thường lớn hơn xe con ở đâu?', a: ['Hai bên thân và phía sau', 'Chỉ trên nóc xe', 'Chỉ phía trước kính lái', 'Không có điểm mù'], correct: 0 },
      { q: 'Tải trọng hàng hóa phải được đặt thế nào?', a: ['Phân bổ đều và không vượt tải', 'Dồn hết về đuôi xe', 'Đặt cao hơn thành xe', 'Không cần chằng buộc'], correct: 0 },
      { q: 'Khi rẽ phải ở giao lộ đông xe, xe tải cần?', a: ['Giảm tốc, quan sát toàn bộ thân xe', 'Rẽ gấp', 'Bấm còi và tăng tốc', 'Tắt xi nhan'], correct: 0 }
    ],
    pilot: [
      { q: 'Trước khi cất cánh cần kiểm tra điều gì?', a: ['Nhiên liệu, thiết bị và đường băng', 'Chỉ màu sơn', 'Radio của người khác', 'Không cần kiểm tra'], correct: 0 },
      { q: 'Khi hạ cánh, phi công phải ưu tiên?', a: ['Giữ tốc độ và độ cao ổn định', 'Tắt toàn bộ liên lạc', 'Bay ngược chiều gió bất kỳ', 'Hạ cánh ngoài đường băng'], correct: 0 },
      { q: 'Khu vực nào cần tránh khi bay trong thành phố?', a: ['Vùng cấm bay được đánh dấu', 'Sân bay được cấp phép', 'Đường bay được chỉ định', 'Khu vực hạ cánh'], correct: 0 },
      { q: 'Khi thời tiết xấu, lựa chọn an toàn là?', a: ['Hạ cánh hoặc chờ chỉ dẫn', 'Bay thấp hơn', 'Tăng tốc xuyên mây', 'Tắt radar'], correct: 0 },
      { q: 'Liên lạc radio giúp phi công?', a: ['Nhận chỉ dẫn và tránh va chạm', 'Tăng sức mạnh động cơ', 'Mở cửa máy bay', 'Thay đổi biển số'], correct: 0 }
    ],
    weapon: [
      { q: 'Khi cầm vũ khí, nòng súng luôn phải hướng về đâu?', a: ['Hướng an toàn', 'Người gần nhất', 'Lên trời trong khu dân cư', 'Bất kỳ hướng nào'], correct: 0 },
      { q: 'Giấy phép vũ khí có ý nghĩa gì?', a: ['Cho phép sở hữu theo quy định IC', 'Cho phép nổ súng mọi nơi', 'Miễn mọi trách nhiệm', 'Thay thế ID cá nhân'], correct: 0 },
      { q: 'Trước khi sử dụng vũ khí cần kiểm tra?', a: ['Tình trạng an toàn và mục tiêu', 'Chỉ màu vũ khí', 'Giá mua', 'Radio'], correct: 0 },
      { q: 'Khi không sử dụng, vũ khí nên được?', a: ['Cất giữ an toàn, tránh người không phận sự', 'Đặt giữa đường', 'Đưa cho bất kỳ ai', 'Để mở khóa trên xe'], correct: 0 },
      { q: 'Nổ súng trong thành phố cần căn cứ vào?', a: ['Luật IC và tình huống chính đáng', 'Sở thích cá nhân', 'Âm thanh radio', 'Thời tiết'], correct: 0 }
    ]
  };

  const TheoryTest = {
    state: { type: 'car', currentQuestion: 0, selectedAnswer: null },

    init() {
      this.cacheElements();
      this.bindEvents();
      this.render();
      window.onCEF('license.open', (type) => {
        if (QUESTION_BANKS[type]) this.state.type = type;
        this.state.currentQuestion = 0;
        this.state.selectedAnswer = null;
        this.render();
      });
    },

    cacheElements() {
      this.elements = {
        options: document.getElementById('answersGrid'),
        nextBtn: document.getElementById('nextQuestionBtn'),
        exitBtn: document.getElementById('exitTheoryBtn'),
        questionStep: document.getElementById('questionStep'),
        progressBar: document.getElementById('progressBar'),
        questionText: document.getElementById('questionText')
      };
    },

    bindEvents() {
      this.elements.nextBtn.addEventListener('click', () => this.nextQuestion());
      this.elements.exitBtn.addEventListener('click', () => window.emitCEF('ui.screen.close', 'theory_test'));
    },

    questions() { return QUESTION_BANKS[this.state.type] || QUESTION_BANKS.car; },

    render() {
      const questions = this.questions();
      const question = questions[this.state.currentQuestion];
      this.elements.questionStep.textContent = `Câu ${this.state.currentQuestion + 1} / ${questions.length}`;
      this.elements.progressBar.style.width = `${((this.state.currentQuestion + 1) / questions.length) * 100}%`;
      this.elements.questionText.textContent = question.q;
      this.elements.options.innerHTML = question.a.map((answer, index) => `<button class="answer-option${this.state.selectedAnswer === index ? ' is-selected' : ''}" data-answer="${index}" type="button"><span class="opt-key">${String.fromCharCode(65 + index)}</span><span class="opt-text">${answer}</span></button>`).join('');
      this.elements.options.querySelectorAll('[data-answer]').forEach((option) => option.addEventListener('click', () => {
        this.state.selectedAnswer = Number(option.dataset.answer);
        this.render();
        window.emitCEF('license.answer', this.state.currentQuestion, this.state.selectedAnswer, this.state.type);
      }));
    },

    nextQuestion() {
      if (this.state.selectedAnswer === null) return;
      const questions = this.questions();
      if (this.state.currentQuestion >= questions.length - 1) {
        window.emitCEF('license.finishTheory', this.state.type);
        return;
      }
      this.state.currentQuestion += 1;
      this.state.selectedAnswer = null;
      this.render();
      window.emitCEF('license.nextQuestion', this.state.currentQuestion, this.state.type);
    }
  };

  document.addEventListener('DOMContentLoaded', () => TheoryTest.init());
})();
