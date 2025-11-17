const API_BASE = 'http://localhost:8000/api';


//JS прави HTTP заявка към бекенда за трансфер на пари
async function transferMoney(from, to, amount) {
    const response = await fetch(`${API_BASE}/transfer`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            from_account_id: parseInt(from),
            to_account_id: parseInt(to),
            amount: parseFloat(amount)
        })
    });

    return await response.json();
}

async function loadAccounts() {
    const tbody = document.getElementById("accounts-tbody");

    try {
        const res = await fetch(`${API_BASE}/accounts`);
        const data = await res.json();

        if (!data.success) {
            tbody.innerHTML = `<tr><td colspan="4">Error</td></tr>`;
            return;
        }

        tbody.innerHTML = data.accounts.map(a => `
            <tr>21312
                <td>${a.owner_name}</td>
                <td>${a.balance.toFixed(2)} lv</td>
                <td>${a.is_active ? "Active" : "Inactive"}</td>
                <td>${new Date(a.created_at * 1000).toLocaleString()}</td>
            </tr>
        `).join("");

    } catch (err) {
        tbody.innerHTML = `<tr><td colspan="4">Error loading</td></tr>`;
    }
}

function showMessage(text, type) {
    const msg = document.getElementById("transfer-message");
    msg.textContent = text;
    msg.className = `form-message ${type}`;
    msg.style.display = "block";
    setTimeout(() => msg.style.display = "none", 3000);
}

//събира даннит и извиква API-то за трансфер
async function handleTransfer(e) {
    e.preventDefault();

    const from = document.getElementById("from-account-id").value;
    const to = document.getElementById("to-account-id").value;
    const amount = document.getElementById("transfer-amount").value;

    const btn = document.getElementById("transfer-btn");
    btn.disabled = true;
    btn.textContent = "Processing...";

    const result = await transferMoney(from, to, amount);

    btn.disabled = false;
    btn.textContent = "Make the transfer";

    if (result.success) {
        showMessage("Transfer successful!", "success");
        loadAccounts();
    } else {
        showMessage("Transfer failed: " + (result.error || result.message), "error");
    }
}

document.addEventListener("DOMContentLoaded", () => {
    loadAccounts();
    document.getElementById("transfer-form")
        .addEventListener("submit", handleTransfer);
});
