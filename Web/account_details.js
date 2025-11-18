const API_BASE = 'http://localhost:8000/api';

let currentAccountId = null;


// Show message
function showMessage(message, type) {
    const messageDiv = document.getElementById('account-message');
    messageDiv.textContent = message;
    messageDiv.className = `form-message ${type}`;
    messageDiv.style.display = 'block';

    setTimeout(() => {
        messageDiv.style.display = 'none';
    }, 5000);
}

// Format date
function formatDate(timestamp) {
    const date = new Date(timestamp * 1000);
    return date.toLocaleString();
}



// Get account by ID
async function getAccount(accountId) {
    try {
        console.log('Fetching account:', accountId);

        // Use /api/accounts and filter client-side
        const response = await fetch(`${API_BASE}/accounts`, {
            cache: 'no-cache'
        });

        const data = await response.json();
        console.log('All accounts:', data);

        if (data.success && data.accounts) {
            const account = data.accounts.find(acc => acc.id === parseInt(accountId));
            if (account) {
                return { success: true, account: account };
            } else {
                return { success: false, error: 'Account not found' };
            }
        }

        return { success: false, error: 'Failed to fetch accounts' };
    } catch (error) {
        console.error('Error fetching account:', error);
        return { success: false, error: 'Network error' };
    }
}

// Get transactions for account
async function getTransactions(accountId) {
    try {
        console.log('Fetching transactions for account:', accountId);

        const response = await fetch(`${API_BASE}/transactions?account_id=${accountId}`, {
            cache: 'no-cache'
        });

        const data = await response.json();
        console.log('Transactions data:', data);

        return data;
    } catch (error) {
        console.error('Error fetching transactions:', error);
        return { success: false, error: 'Network error' };
    }
}

// Update account
async function updateAccount(accountId, ownerName, isActive) {
    try {
        console.log('Updating account:', { accountId, ownerName, isActive });

        const response = await fetch(`${API_BASE}/account/${accountId}`, {
            method: 'PUT',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                owner_name: ownerName,
                is_active: parseInt(isActive)
            })
        });

        const data = await response.json();
        console.log('Update response:', data);

        return data;
    } catch (error) {
        console.error('Error updating account:', error);
        return { success: false, error: 'Network error' };
    }
}




// Display account details
function displayAccountDetails(account) {
    document.getElementById('display-account-id').textContent = account.id;
    document.getElementById('display-owner-name').textContent = account.owner_name;
    document.getElementById('display-balance').textContent = `${account.balance.toFixed(2)} lv`;

    const statusElement = document.getElementById('display-status');
    statusElement.textContent = account.is_active ? '✓ Active' : '✗ Inactive';
    statusElement.className = account.is_active ? 'info-value status-active' : 'info-value status-inactive';

    document.getElementById('display-created-at').textContent = formatDate(account.created_at);

    // Show the account info container
    document.getElementById('account-info-container').style.display = 'block';

    // Store current account ID
    currentAccountId = account.id;
}

// Display transactions
function displayTransactions(transactions) {
    const tbody = document.getElementById('transactions-tbody');

    if (!transactions || transactions.length === 0) {
        tbody.innerHTML = '<tr><td colspan="4">No transactions found</td></tr>';
        return;
    }

    tbody.innerHTML = '';

    transactions.forEach(tx => {
        const row = tbody.insertRow();

        // ID
        row.insertCell(0).textContent = tx.id;

        // Type
        const typeCell = row.insertCell(1);
        let typeText = '';
        let typeClass = '';

        switch (tx.type) {
            case 0: // DEPOSIT (защото при теб е 0)
                typeText = '💰 Deposit';
                typeClass = 'transaction-deposit';
                break;
            case 1:
                typeText = '💸 Withdraw';
                typeClass = 'transaction-withdraw';
                break;
            case 2:
                typeText = '⬇️ Transfer In';
                typeClass = 'transaction-transfer-in';
                break;
            case 3:
                typeText = '⬆️ Transfer Out';
                typeClass = 'transaction-transfer-out';
                break;
            default:
                typeText = 'Unknown';
        }


        typeCell.textContent = typeText;
        typeCell.className = typeClass;

        // Amount
        row.insertCell(2).textContent = `${tx.amount.toFixed(2)} lv`;

        // Date
        row.insertCell(3).textContent = formatDate(tx.timestamp);
    });
}

// Display statistics
function displayStatistics(transactions) {
    let totalDeposits = 0;
    let totalWithdrawals = 0;
    let transactionCount = transactions ? transactions.length : 0;

    if (transactions) {
        transactions.forEach(tx => {
            if (tx.type === 0 || tx.type === 3) {
                totalDeposits += tx.amount;
            } else if (tx.type === 1  || tx.type === 4) {
                totalWithdrawals += tx.amount;
            }
        });
    }

    document.getElementById('total-deposits').textContent = `${totalDeposits.toFixed(2)} lv`;
    document.getElementById('total-withdrawals').textContent = `${totalWithdrawals.toFixed(2)} lv`;
    document.getElementById('transaction-count').textContent = transactionCount;
}


// Handle account lookup
async function handleAccountLookup(e) {
    e.preventDefault();

    const accountId = document.getElementById('account-id').value;

    if (!accountId || accountId <= 0) {
        showMessage('✗ Please enter a valid account ID', 'error');
        return;
    }

    console.log('Looking up account:', accountId);

    // Fetch account details
    const accountResult = await getAccount(accountId);

    if (accountResult.success && accountResult.account) {
        displayAccountDetails(accountResult.account);
        showMessage('✓ Account loaded successfully', 'success');

        // Fetch and display transactions
        const txResult = await getTransactions(accountId);
        if (txResult.success) {
            displayTransactions(txResult.transactions);
            displayStatistics(txResult.transactions);
        }
    } else {
        showMessage(`✗ Account not found: ${accountResult.error || 'Unknown error'}`, 'error');
        document.getElementById('account-info-container').style.display = 'none';
    }
}

// Show edit form
function showEditForm() {
    const editSection = document.getElementById('edit-account-section');
    editSection.style.display = 'block';

    // Pre-fill form with current values
    const ownerName = document.getElementById('display-owner-name').textContent;
    const status = document.getElementById('display-status').textContent.includes('Active') ? '1' : '0';

    document.getElementById('edit-owner-name').value = ownerName;
    document.getElementById('edit-account-status').value = status;
}

// Hide edit form
function hideEditForm() {
    document.getElementById('edit-account-section').style.display = 'none';
}

// Handle edit account
async function handleEditAccount(e) {
    e.preventDefault();

    if (!currentAccountId) {
        showMessage('✗ No account selected', 'error');
        return;
    }

    const ownerName = document.getElementById('edit-owner-name').value;
    const isActive = document.getElementById('edit-account-status').value;

    console.log('Updating account:', { currentAccountId, ownerName, isActive });

    const result = await updateAccount(currentAccountId, ownerName, isActive);

    if (result.success) {
        showMessage('✓ Account updated successfully', 'success');
        hideEditForm();

        // Reload account details
        const accountResult = await getAccount(currentAccountId);
        if (accountResult.success && accountResult.account) {
            displayAccountDetails(accountResult.account);
        }
    } else {
        showMessage(`✗ Update failed: ${result.error || 'Unknown error'}`, 'error');
    }
}


document.addEventListener('DOMContentLoaded', () => {
    console.log('Account details page loaded');

    // Attach event listeners
    document.getElementById('account-lookup-form').addEventListener('submit', handleAccountLookup);
    document.getElementById('edit-account-btn').addEventListener('click', showEditForm);
    document.getElementById('cancel-edit-btn').addEventListener('click', hideEditForm);
    document.getElementById('edit-account-form').addEventListener('submit', handleEditAccount);
    document.getElementById('delete-account-btn').addEventListener('click', handleDeleteAccount);

    console.log('Event listeners attached');
});