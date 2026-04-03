/**
 * app.js  –  Application controller
 * Handles page routing, form events, UI updates.
 */

// ─── Session ──────────────────────────────────────────────────
const Session = {
  set(user) { localStorage.setItem("dp_session", JSON.stringify(user)); },
  get()     { try { return JSON.parse(localStorage.getItem("dp_session")); } catch { return null; } },
  clear()   { localStorage.removeItem("dp_session"); },
  isLoggedIn() { return !!this.get(); }
};

// ─── Routing ──────────────────────────────────────────────────
const Router = {
  go(page) {
    document.querySelectorAll(".page").forEach(p => p.classList.remove("active"));
    const el = document.getElementById("page-" + page);
    if (el) el.classList.add("active");
    window.scrollTo(0, 0);
  }
};

// ─── Toast ────────────────────────────────────────────────────
function toast(msg, type = "info") {
  const t = document.getElementById("toast");
  t.textContent = msg;
  t.className = "toast show " + type;
  setTimeout(() => t.classList.remove("show"), 3500);
}

// ─── Loading state ────────────────────────────────────────────
function setLoading(btnId, loading) {
  const btn = document.getElementById(btnId);
  if (!btn) return;
  btn.disabled = loading;
  btn.dataset.orig = btn.dataset.orig || btn.textContent;
  btn.textContent = loading ? "Please wait…" : btn.dataset.orig;
}

// ─── OTP countdown timer ─────────────────────────────────────
let otpTimer = null;
function startOTPTimer(seconds = 300) {
  clearInterval(otpTimer);
  const el = document.getElementById("otp-timer");
  if (!el) return;
  let s = seconds;
  const tick = () => {
    el.textContent = `OTP expires in ${Math.floor(s/60)}:${String(s%60).padStart(2,"0")}`;
    if (--s < 0) { clearInterval(otpTimer); el.textContent = "OTP expired. Request again."; }
  };
  tick();
  otpTimer = setInterval(tick, 1000);
}

// ─── Pending OTP state ────────────────────────────────────────
let pendingOTP   = null;
let pendingEmail = null;
let pendingFlow  = null;  // "register" | "login"

// ═══════════════════════════════════════════════════════════════
//  SIGNUP FLOW
// ═══════════════════════════════════════════════════════════════
async function handleRequestSignupOTP() {
  const email = document.getElementById("signup-email").value.trim();
  if (!email.match(/^[^\s@]+@[^\s@]+\.[^\s@]+$/)) {
    toast("Enter a valid email address.", "error"); return;
  }
  setLoading("btn-request-otp", true);
  const res = await API.requestOTP(email);
  setLoading("btn-request-otp", false);

  if (res.status !== "ok") { toast(res.message, "error"); return; }

  pendingOTP   = res.otp;    // demo only – normally hidden
  pendingEmail = email;
  pendingFlow  = "register";

  document.getElementById("otp-email-display").textContent = email;
  toast(`OTP sent! (Demo: check browser console)`, "info");
  // Auto-fill OTP field in demo mode
  document.getElementById("signup-otp").value = res.otp;
  startOTPTimer();

  showStep("signup", 2);
}

async function handleRegister() {
  const name     = document.getElementById("signup-name").value.trim();
  const age      = document.getElementById("signup-age").value;
  const weight   = document.getElementById("signup-weight").value;
  const height   = document.getElementById("signup-height").value;
  const goal     = document.getElementById("signup-goal").value;
  const pwd      = document.getElementById("signup-password").value;
  const otp      = document.getElementById("signup-otp").value.trim();

  if (!name || !age || !weight || !height || !pwd || !otp) {
    toast("Please fill all fields.", "error"); return;
  }

  setLoading("btn-register", true);
  const res = await API.register({
    name, age, weight, height, goal,
    email: pendingEmail, password: pwd, otp
  });
  setLoading("btn-register", false);

  if (res.status !== "ok") { toast(res.message, "error"); return; }

  toast("Account created! Please log in.", "success");
  clearInterval(otpTimer);
  setTimeout(() => Router.go("login"), 1500);
}

// ═══════════════════════════════════════════════════════════════
//  LOGIN FLOW
// ═══════════════════════════════════════════════════════════════
async function handleRequestLoginOTP() {
  const email = document.getElementById("login-email").value.trim();
  const pwd   = document.getElementById("login-password").value;
  if (!email || !pwd) { toast("Enter email and password.", "error"); return; }

  setLoading("btn-login-otp", true);
  const res = await API.loginOTP(email, pwd);
  setLoading("btn-login-otp", false);

  if (res.status !== "ok") { toast(res.message, "error"); return; }

  pendingEmail = email;
  pendingFlow  = "login";

  document.getElementById("login-otp").value = res.otp;   // demo autofill
  toast("OTP sent! (Demo: check browser console)", "info");
  startOTPTimer();
  document.getElementById("login-otp-section").classList.remove("hidden");
}

async function handleVerifyLogin() {
  const otp = document.getElementById("login-otp").value.trim();
  if (!otp) { toast("Enter the OTP.", "error"); return; }

  setLoading("btn-verify-login", true);
  const res = await API.verifyLogin(pendingEmail, otp);
  setLoading("btn-verify-login", false);

  if (res.status !== "ok") { toast(res.message, "error"); return; }

  Session.set(res);
  clearInterval(otpTimer);
  toast(`Welcome back, ${res.name}!`, "success");
  setTimeout(() => loadDashboard(), 500);
}

// ═══════════════════════════════════════════════════════════════
//  DASHBOARD
// ═══════════════════════════════════════════════════════════════
async function loadDashboard() {
  const user = Session.get();
  if (!user) { Router.go("login"); return; }

  Router.go("dashboard");

  // Load progress
  const prog = await API.getProgress(user.userID);
  if (prog.status !== "ok") return;

  document.getElementById("dash-name").textContent         = prog.name;
  document.getElementById("dash-goal").textContent         = goalLabel(prog.goal);
  document.getElementById("dash-bmi").textContent          = prog.bmi;
  document.getElementById("dash-bmi-cat").textContent      = prog.bmiCategory;
  document.getElementById("dash-target-cal").textContent   = prog.targetCalories;
  document.getElementById("dash-logged-cal").textContent   = Math.round(prog.loggedCalories);
  document.getElementById("dash-remaining-cal").textContent= Math.max(0, Math.round(prog.remainingCalories));
  document.getElementById("dash-total-plans").textContent  = prog.totalPlans;
  document.getElementById("dash-weight").textContent       = prog.weight + " kg";
  document.getElementById("dash-height").textContent       = prog.height + " cm";

  // Progress bar
  const pct = Math.min(100, Math.round((prog.loggedCalories / prog.targetCalories) * 100));
  document.getElementById("cal-bar").style.width = pct + "%";
  document.getElementById("cal-bar-pct").textContent = pct + "%";

  // BMI visual
  renderBMIGauge(prog.bmi);

  // Load recent plan if any
  await loadLatestPlan();
}

async function loadLatestPlan() {
  const user = Session.get();
  const res  = await API.getHistory(user.userID);
  if (res.status !== "ok" || !res.plans.length) return;
  const latest = res.plans[res.plans.length - 1];
  renderPlan(latest, "current-plan-card");
}

// ═══════════════════════════════════════════════════════════════
//  DIET PLAN PAGE
// ═══════════════════════════════════════════════════════════════
async function handleGeneratePlan() {
  const user = Session.get();
  if (!user) return;
  setLoading("btn-generate-plan", true);
  const res = await API.generatePlan(user.userID);
  setLoading("btn-generate-plan", false);
  if (res.status !== "ok") { toast(res.message, "error"); return; }
  toast("New diet plan generated!", "success");
  renderPlan(res, "plan-display");
  await loadDashboard();
}

async function handleLoadHistory() {
  const user = Session.get();
  const res  = await API.getHistory(user.userID);
  if (res.status !== "ok") return;
  const container = document.getElementById("history-list");
  container.innerHTML = "";
  if (!res.plans.length) {
    container.innerHTML = "<p class='empty'>No plans yet. Generate your first plan!</p>";
    return;
  }
  res.plans.slice().reverse().forEach(plan => {
    const card = document.createElement("div");
    card.className = "history-card";
    card.innerHTML = `
      <div class="hc-header">
        <span class="hc-goal ${plan.goal}">${goalLabel(plan.goal)}</span>
        <span class="hc-date">${plan.date}</span>
        <span class="hc-cal">${plan.totalCalories} kcal</span>
      </div>
      <button class="btn-sm" onclick='renderPlan(${JSON.stringify(plan)}, "plan-display"); document.getElementById("plan-display").scrollIntoView({behavior:"smooth"})'>
        View Full Plan
      </button>`;
    container.appendChild(card);
  });
}

// ─── Render a plan into a target element ─────────────────────
function renderPlan(plan, targetId) {
  const el = document.getElementById(targetId);
  if (!el) return;

  const mealHTML = plan.meals.map(m => `
    <div class="meal-card">
      <div class="meal-header">
        <span class="meal-icon">${mealIcon(m.type)}</span>
        <span class="meal-type">${m.type}</span>
        <span class="meal-cal">${m.calories} kcal</span>
      </div>
      <p class="meal-foods">${m.foods}</p>
      <div class="macro-row">
        <span class="macro protein">P: ${m.p || m.protein}g</span>
        <span class="macro carbs">C: ${m.c || m.carbs}g</span>
        <span class="macro fats">F: ${m.f || m.fats}g</span>
      </div>
    </div>`).join("");

  el.innerHTML = `
    <div class="plan-header">
      <h3>${goalLabel(plan.goal)} Plan</h3>
      <span class="plan-date">${plan.date}</span>
    </div>
    <div class="plan-macros">
      <div class="macro-stat"><span>${plan.totalCalories}</span><small>Total kcal</small></div>
      <div class="macro-stat"><span>${plan.totalProtein}g</span><small>Protein</small></div>
      <div class="macro-stat"><span>${plan.totalCarbs}g</span><small>Carbs</small></div>
      <div class="macro-stat"><span>${plan.totalFats}g</span><small>Fats</small></div>
    </div>
    <div class="meals-grid">${mealHTML}</div>
    <div class="plan-notes">💡 ${plan.notes}</div>`;
}

// ─── Calorie logger ───────────────────────────────────────────
async function handleLogCalories() {
  const cal  = document.getElementById("cal-input").value;
  const user = Session.get();
  if (!cal || cal <= 0) { toast("Enter a valid calorie amount.", "error"); return; }
  const res = await API.logCalories(user.userID, +cal);
  if (res.status !== "ok") { toast(res.message, "error"); return; }
  toast(`${cal} kcal logged!`, "success");
  document.getElementById("cal-input").value = "";
  await loadDashboard();
}

// ─── Profile update ───────────────────────────────────────────
async function handleUpdateProfile() {
  const weight = document.getElementById("prof-weight").value;
  const height = document.getElementById("prof-height").value;
  const goal   = document.getElementById("prof-goal").value;
  const user   = Session.get();
  if (!weight || !height) { toast("Fill all fields.", "error"); return; }

  setLoading("btn-update-profile", true);
  const res = await API.updateProfile(user.userID, { weight, height, goal });
  setLoading("btn-update-profile", false);

  if (res.status !== "ok") { toast(res.message, "error"); return; }
  toast("Profile updated!", "success");
  // Update session goal
  user.goal = goal;
  Session.set(user);
  await loadDashboard();
}

// ─── BMI Gauge (simple CSS bar) ──────────────────────────────
function renderBMIGauge(b) {
  const el = document.getElementById("bmi-needle");
  if (!el) return;
  // Map BMI 10–40 → 0–100%
  const pct = Math.min(100, Math.max(0, ((b - 10) / 30) * 100));
  el.style.left = pct + "%";
}

// ─── Helpers ─────────────────────────────────────────────────
function goalLabel(g) {
  return { loss:"Weight Loss", gain:"Weight Gain", maintain:"Maintenance" }[g] || g;
}
function mealIcon(type) {
  const t = (type || "").toLowerCase();
  if (t.includes("breakfast"))  return "🌅";
  if (t.includes("lunch"))      return "☀️";
  if (t.includes("dinner"))     return "🌙";
  if (t.includes("snack") || t.includes("pre")) return "🥗";
  return "🍽️";
}

// ─── Multi-step signup ────────────────────────────────────────
function showStep(form, step) {
  document.querySelectorAll(`#page-${form} .step`).forEach((s, i) => {
    s.classList.toggle("active", i + 1 === step);
  });
}

// ─── Logout ───────────────────────────────────────────────────
function logout() {
  Session.clear();
  Router.go("login");
  toast("Logged out.", "info");
}

// ─── Init ─────────────────────────────────────────────────────
document.addEventListener("DOMContentLoaded", () => {
  if (Session.isLoggedIn()) {
    loadDashboard();
  } else {
    Router.go("login");
  }
});
