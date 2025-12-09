let redThreshold = 20;
let greenThreshold = 80;
let sensorEnabled = { 1: true, 2: true };
let ledEnabled = { 1: true, 2: true };

function getColor(percent) {
    if (percent <= redThreshold) return 'red';
    if (percent >= greenThreshold) return 'green';
    const ratio = (percent - redThreshold) / (greenThreshold - redThreshold);
    const r = Math.floor(255 * (1 - ratio));
    const g = Math.floor(255 * ratio);
    return `rgb(${r}, ${g}, 0)`;
}

function updateBar(id, percent) {
    const bar = document.getElementById(`bar${id}`);
    const percentEl = document.getElementById(`percent${id}`);
    if (percent === null || percent < 0) {
        bar.style.width = '0%';
        bar.style.background = '#555';
        percentEl.textContent = '--%';
        return;
    }
    bar.style.width = `${Math.min(100, Math.max(0, percent))}%`;
    bar.style.background = getColor(percent);
    percentEl.textContent = `${Math.round(percent)}%`;
}

function fetchStatus() {
    fetch('/status')
        .then(r => r.json())
        .then(data => {
            updateBar(1, data.enabled['1'] ? data.moisture['1'] : null);
            updateBar(2, data.enabled['2'] ? data.moisture['2'] : null);
            document.getElementById('status').textContent = 'OK';
        })
        .catch(() => {
            document.getElementById('status').textContent = 'Error';
        });
}

function applyThresholds() {
    redThreshold = parseInt(document.getElementById('threshold-red').value) || 0;
    greenThreshold = parseInt(document.getElementById('threshold-green').value) || 100;
    if (greenThreshold < redThreshold) greenThreshold = redThreshold;
    sendControl();
}

function sendControl() {
    const params = new URLSearchParams({
        red: redThreshold,
        green: greenThreshold,
        s1: sensorEnabled[1] ? '1' : '0',
        s2: sensorEnabled[2] ? '1' : '0',
        l1: ledEnabled[1] ? '1' : '0',
        l2: ledEnabled[2] ? '1' : '0'
    });
    fetch(`/control?${params}`)
        .then(() => fetchStatus())
        .catch(() => {
            document.getElementById('status').textContent = 'Control error';
        });
}

function bindUI() {
    document.getElementById('apply-thresholds').onclick = applyThresholds;

    ['1', '2'].forEach(id => {
        const s = document.getElementById(`sensor${id}-enable`);
        const l = document.getElementById(`led${id}-enable`);
        s.onchange = () => { sensorEnabled[id] = s.checked; sendControl(); };
        l.onchange = () => { ledEnabled[id] = l.checked; sendControl(); };
    });
}

document.addEventListener('DOMContentLoaded', () => {
    bindUI();
    fetchStatus();
    setInterval(fetchStatus, 1000);
});