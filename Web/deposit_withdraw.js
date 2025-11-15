    const API_BASE = 'http://localhost:8000/api';

    function showMessage(elementId, message, type) {
        const messageDiv = document.getElementById(elementId);
        messageDiv.textContent = message;
        messageDiv.className = `form-message ${type}`;
        messageDiv.style.display = 'block';
        setTimeout(() => {
            messageDiv.style.display = 'none';
        }, 5000);
    }

    async function depositMoney(accountId, amount) {

        try {
            console.log("Sending deposit request:", { accountId, amount });

            const response = await fetch(`${API_BASE}/deposit`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body:
                    JSON.stringify({
                        account_id: parseInt(accountId),
                        amount: parseFloat(amount)
                    })

            });

            const data = await response.json();
            console.log("Deposit response data:", data);

            return data;
        }
        catch (error) {
            console.error("Error during deposit:", error);
            return { success: false, message: "Network error during deposit." };
        }
    }


    async function withdrawMoney(accountId, amount) {
        try {

            console.log("Sending withdrawal request:", { accountId, amount });

            const response = await fetch(`${API_BASE}/withdraw`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body:
                    JSON.stringify({
                        account_id: parseInt(accountId),
                        amount: parseFloat(amount)
                    })
            });

            const data = await response.json();
            console.log("Withdrawal response data:", data);

            return data;
        }
        catch (error) {
            console.error("Error during withdrawal:", error);
            return { success: false, message: "Network error during withdrawal." };
        }
    }

    async function loadAccountsList() {

        const listDiv = document.getElementById('accounts-list');


        try {
            const response = await fetch(`${API_BASE}/accounts`);
            const data = await response.json();

            if (data.success && data.accounts.length > 0) {
                listDiv.innerHTML = '';

                data.accounts.forEach(account => {
                    const card = document.createElement('div');
                    card.className = 'account-card';
                    card.innerHTML = `
                <h3>Account ID: ${account.id}</h3>
                <p>Owner: ${account.owner_name}</p>
                <p>Balance: ${account.balance.toFixed(2)} lv</p>
            `;
                card.addEventListener('click', (event) => fillAccountId(event, account.id));
                    listDiv.appendChild(card);
                });
            } else {
                listDiv.innerHTML = '<div disabled>No accounts available</div>';
            }
        }
        catch (error) {
            console.error("Error loading data:", error);
        }
    }

    function fillAccountId(event, id) {
        console.log('Selected account ID:', id);

        document.getElementById('deposit-account-id').value = id;
        document.getElementById('withdraw-account-id').value = id;

        const cards = document.querySelectorAll('.account-card');
        cards.forEach(card => card.style.borderColor = 'transparent');

        event.currentTarget.style.borderColor = '#667eea';
    }

    async function handleDeposit(e) {
        e.preventDefault();

        const accountId = document.getElementById('deposit-account-id').value;
        const amount = document.getElementById('deposit-amount').value;

        if (!accountId || !amount || parseFloat(amount) <= 0) {
            showMessage('deposit-message', 'Please enter valid account ID and amount.', 'error');
            return;
        }

        const submitBtn = e.target.querySelector('button[type="submit"]');
        const submitOriginalText = submitBtn.textContent;
        submitBtn.disabled = true;
        submitBtn.textContent = 'Processing...';

        const result = await depositMoney(accountId, amount);

        submitBtn.disabled = false;
        submitBtn.textContent = submitOriginalText;

        if (result.success) {
            loadAccountsList();  
            showMessage('deposit-message', result.message, 'success');
        } else {
            showMessage('deposit-message', result.error || "Unknown error", 'error');
        }
    }

    async function handleWithdraw(e) {
        e.preventDefault();
        
        const accountId = document.getElementById('withdraw-account-id').value;
        const amount = document.getElementById('withdraw-amount').value;

        if (!accountId || !amount || parseFloat(amount) <= 0) {
            showMessage('withdraw-message', 'Please enter valid account ID and amount.', 'error');
            return;
        }

        const submitBtn = e.target.querySelector('button[type="submit"]');
        const original = submitBtn.textContent;
        submitBtn.disabled = true;
        submitBtn.textContent = 'Processing...';

        const result = await withdrawMoney(accountId, amount);

        submitBtn.disabled = false;
        submitBtn.textContent = original;

        if (result.success) {
            loadAccountsList();
            showMessage('withdraw-message', result.message, 'success');
        } else {
            showMessage('withdraw-message', result.error || 'Unknown error', 'error');
        }
    }





    document.addEventListener('DOMContentLoaded', () => {
        console.log('Deposit/Withdraw page loaded');
        loadAccountsList();

        const depositForm = document.getElementById('deposit-form');
        depositForm.addEventListener('submit', handleDeposit);

        const withdrawForm = document.getElementById('withdraw-form');
        withdrawForm.addEventListener('submit', handleWithdraw);

        console.log('Event listeners for deposit and withdraw forms set up');

    });