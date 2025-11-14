const API_BASE = "http://localhost:8000/api";


async function loadStats(params) {

    try {

        const response = await fetch(`${API_BASE}/stats`);
        const data = await response.json();


        if (data.success) {
            document.getElementById('total-balance').textContent =
                `${data.stats.total_balance.toFixed(2)} lv`;

            document.getElementById('account-count').textContent =
                data.stats.account_count;

            document.getElementById('average-balance').textContent =
                `${data.stats.average_balance.toFixed(2)} lv`;
        }

    }
    catch (error) {
        console.error("Error loading stats:", error);
        document.getElementById('total-balance').textContent = "Error";
        document.getElementById('account-count').textContent = "Error";
        document.getElementById('average-balance').textContent = "Error";

    }
}

async function loadAccounts() {
    try {
        const response = await fetch(`${API_BASE}/accounts`);

        if (response.status === 404) {
            console.error("No accounts found.");
            return;
        }

        const data = await response.json();


        if (data.success && data.accounts.length > 0) {
            const accountsTableBody = document.getElementById('accounts-tbody');
            accountsTableBody.innerHTML = `${data.accounts.map(account => `
                <tr>
                    <td>${account.owner_name}</td>
                    <td>${account.balance.toFixed(2)} lv</td>
                    <td>${account.is_active ? 'Active' : 'Inactive'}</td>
                    <td>${formatDate(account.created_at)}</td>
                </tr>
            `).join('')}`;
        }
    }
    catch (error) {
        console.error("Error loading accounts:", error);
        const accountsTableBody = document.getElementById('accounts-tbody');
        accountsTableBody.innerHTML = `<tr><td colspan="3">Error loading accounts.</td></tr>`;
    }
}

function formatDate(timestamp) {
    return new Date(timestamp * 1000).toLocaleDateString();
}

async function reloadAll() {
    await loadStats();
    await loadAccounts();
    location.reload();
}

window.addEventListener('DOMContentLoaded', () => {
    loadStats();
    loadAccounts();
    
  
}); 