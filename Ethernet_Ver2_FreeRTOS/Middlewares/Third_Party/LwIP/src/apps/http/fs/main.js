const wishes = [
  "Chúc bạn mỗi ngày đều rực rỡ niềm vui và những cơ hội mới!",
  "Mong mọi điều tốt đẹp và bình an luôn đồng hành cùng bạn.",
  "Ngày hôm nay của bạn sẽ tuyệt diệu hơn cả mong đợi!",
  "Bạn xứng đáng với hạnh phúc vô hạn và những giấc mơ trọn vẹn.",
  "Gửi bạn năng lượng tích cực và một bầu trời may mắn.",
  "Hãy luôn tin vào bản thân, điều kỳ diệu đang đến gần!",
  "Chúc bạn sức khỏe dồi dào, tâm an, lòng sáng!",
  "Bạn là nguồn cảm hứng — hãy tiếp tục lan tỏa yêu thương.",
  "Tương lai của bạn đang sáng bừng, hãy bước tới tự tin!",
  "Cảm ơn vì đã là chính bạn, thế giới này cần bạn!"
];

const page = document.body.dataset.page || "card";
const toastStack = document.getElementById("toastStack");

const showToast = (message, variant = "info") => {
  if (!toastStack) return;
  const el = document.createElement("div");
  el.className = `toast ${variant}`;
  el.textContent = message;
  toastStack.appendChild(el);
  requestAnimationFrame(() => el.classList.add("show"));
  setTimeout(() => {
    el.classList.remove("show");
    setTimeout(() => el.remove(), 300);
  }, 2400);
};

const randomWish = (exclude) => {
  const pool = exclude ? wishes.filter((w) => w !== exclude) : wishes;
  return pool[Math.floor(Math.random() * pool.length)];
};

const buildCardUrl = (text) => new URL(`card.html?msg=${encodeURIComponent(text)}`, window.location.href).href;
const buildQrUrl = (data) => `https://api.qrserver.com/v1/create-qr-code/?size=320x320&data=${encodeURIComponent(data)}`;

function setupGate() {
  const qrImg = document.getElementById("qrOnlyImg");
  const shuffleBtn = document.getElementById("shuffleBtn");
  const createBtn = document.getElementById("createBtn");
  const downloadBtn = document.getElementById("downloadBtn");
  const shareBtn = document.getElementById("shareBtn");
  const copyQrBtn = document.getElementById("copyQrBtn");
  const customMsg = document.getElementById("customMsg");

  let currentWish = randomWish();
  let currentLink = "";
  let pendingSource = "init";

  const render = (wish, source = "init") => {
    if (customMsg) customMsg.value = wish;
    const link = buildCardUrl(wish);
    currentLink = link;
    qrImg.classList.remove("visible");
    qrImg.src = buildQrUrl(link);
    currentWish = wish;
    pendingSource = source;
  };

  qrImg?.addEventListener("load", () => {
    qrImg.classList.add("visible");
    if (pendingSource === "create") {
      showToast("Đã tạo mã QR thành công", "success");
    }
  });

  qrImg?.addEventListener("error", () => {
    showToast("Tạo mã QR thất bại, thử lại nhé", "error");
  });

  shuffleBtn?.addEventListener("click", () => {
    const next = randomWish(currentWish);
    render(next, "shuffle");
  });

  createBtn?.addEventListener("click", () => {
    const text = (customMsg?.value || "").trim();
    render(text || randomWish(), "create");
    showToast("Đang tạo mã QR...", "info");
  });

  downloadBtn?.addEventListener("click", async () => {
    if (!qrImg?.src) return;
    try {
      const res = await fetch(qrImg.src);
      const blob = await res.blob();
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = "qr-thiep.png";
      document.body.appendChild(a);
      a.click();
      a.remove();
      URL.revokeObjectURL(url);
      downloadBtn.textContent = "Đã tải ✔";
      showToast("Đã tải mã QR", "success");
    } catch (err) {
      downloadBtn.textContent = "Tải không thành công";
      showToast("Không tải được, thử lại nhé", "error");
    }
    setTimeout(() => (downloadBtn.textContent = "Tải QR"), 1400);
  });

  const copyQrImage = async () => {
    if (!qrImg?.src) return;
    try {
      const res = await fetch(qrImg.src);
      const blob = await res.blob();
      if (navigator.clipboard?.write && window.ClipboardItem) {
        const item = new ClipboardItem({ [blob.type || "image/png"]: blob });
        await navigator.clipboard.write([item]);
        showToast("Đã sao chép QR, dán được vào Zalo/Messenger", "success");
      } else if (navigator.clipboard?.writeText) {
        await navigator.clipboard.writeText(currentLink);
        showToast("Thiết bị chưa hỗ trợ dán ảnh; đã sao chép link", "warn");
      } else {
        showToast("Thiết bị không cho phép sao chép", "error");
      }
    } catch (err) {
      showToast("Sao chép không thành công", "error");
    }
  };

  copyQrBtn?.addEventListener("click", copyQrImage);

  shareBtn?.addEventListener("click", async () => {
    if (!currentLink) return;
    if (navigator.share) {
      try {
        const res = await fetch(qrImg.src);
        const blob = await res.blob();
        const file = new File([blob], "qr-thiep.png", { type: blob.type || "image/png" });
        const shareData = {
          title: "Thiệp 3D dành cho bạn",
          text: "Quét mã để mở thiệp 3D kèm lời chúc.",
          url: currentLink,
        };
        if (navigator.canShare?.({ files: [file] })) {
          shareData.files = [file];
        }
        await navigator.share(shareData);
        shareBtn.textContent = "Đã chia sẻ ✔";
        showToast("Đã chia sẻ mã QR", "success");
      } catch (err) {
        shareBtn.textContent = "Không chia sẻ được";
        showToast("Chia sẻ không thành công", "error");
      }
    } else if (navigator.clipboard) {
      try {
        await copyQrImage();
        shareBtn.textContent = "Đã sao chép ✔";
      } catch (err) {
        shareBtn.textContent = "Không sao chép được";
      }
    }
    setTimeout(() => (shareBtn.textContent = "Chia sẻ"), 1400);
  });

  render(currentWish);
}

function setupCard() {
  const params = new URLSearchParams(window.location.search);
  const raw = params.get("msg");
  let current = raw ? decodeURIComponent(raw) : randomWish();

  const letterText = document.getElementById("letterText");
  const cardShell = document.getElementById("cardShell");
  const cardOuter = document.getElementById("cardOuter");
  const hintBtn = document.getElementById("hintBtn");

  const updateLetter = (text) => {
    if (!letterText) return;
    letterText.textContent = text;
    requestAnimationFrame(() => {
      const paper = letterText.closest(".letter");
      if (!paper) return;
      const body = letterText;
      const contentHeight = body.scrollHeight;
      const base = 360;
      const max = 360;
      const padded = Math.min(Math.max(contentHeight + 40, base), max);
      paper.style.setProperty("--letter-height", `${padded}px`);
    });
  };

  const toggleOpen = () => {
    const isOpen = cardShell?.classList.toggle("open");
    if (isOpen) {
      if (cardOuter) cardOuter.style.transform = "";
    } else if (cardOuter) {
      cardOuter.style.transform = "";      
    }
  };

  cardOuter?.addEventListener("click", toggleOpen);
  hintBtn?.addEventListener("click", toggleOpen);

  updateLetter(current);
}

if (page === "gate") {
  setupGate();
} else {
  setupCard();
}
