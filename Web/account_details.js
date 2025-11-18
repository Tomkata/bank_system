const API_BASE = 'http://localhost:8000/api';

let currentAccountId = null;
let allTransactions
= [];
let currPage = 1;
const perPage = 10;


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
        if(data.success){
            allTransactions = data.transactions || [];
            currPage = 1;

            displayCurrentPage();
            updatePaginationControls();
        }
        console.log('Transactions data:', data);



        return data;
    } catch (error) {
        console.error('Error fetching transactions:', error);
        return { success: false, error: 'Network error' };
    }
}


function displayCurrentPage() {

    const totalPages = Math.ceil(allTransactions.length / perPage);
    const startIndex = (currPage - 1) * perPage;
    const endIndex = startIndex + perPage;
    const pageTransactions = allTransactions.slice(startIndex, endIndex);

    displayTransactions(pageTransactions);

}

function updatePaginationControls() {
    const totalPages = Math.ceil(allTransactions.length / perPage);
document.getElementById('page-info').textContent = `Page ${currPage} of ${totalPages}`;

    document.getElementById('prev-page-btn').disabled = currPage === 1;
    document.getElementById('next-page-btn').disabled = currPage === totalPages;

}

function   handlePreviousPage() {
    if (currPage > 1) {
        currPage--;
        displayCurrentPage();
        updatePaginationControls();

    }
}

function handleNextPage() {
    const totalPages = Math.ceil(allTransactions.length / perPage);
    if (currPage < totalPages) {
        currPage++;
        displayCurrentPage();
                updatePaginationControls();

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
            } else if (tx.type === 1 || tx.type === 4) {
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
            // displayCurrentPage() and updatePaginationControls() are called in getTransactions()
            displayStatistics(allTransactions);
        }
    } else {
        showMessage(`✗ Account not found: ${accountResult.error || 'Unknown error'}`, 'error');
        document.getElementById('account-info-container').style.display = 'none';
    }
}



document.addEventListener('DOMContentLoaded', () => {
    

       console.log('prev-page-btn:', document.getElementById('prev-page-btn'));
    console.log('next-page-btn:', document.getElementById('next-page-btn'));
    console.log('page-info:', document.getElementById('page-info'));
  
    // Attach event listeners
    document.getElementById('account-lookup-form').addEventListener('submit', handleAccountLookup);


    document.getElementById('next-page-btn').addEventListener('click', handleNextPage);
  document.getElementById('prev-page-btn').addEventListener('click', handlePreviousPage);


    console.log('Event listeners attached');
});