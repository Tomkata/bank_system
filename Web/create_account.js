const API_BASE = 'http://localhost:8000/api';


// Функция за показване на съобщения
function showMessage(message, type) {
    // Създаваме div за съобщението ако не съществува
    let messageDiv = document.getElementById('create-account-message');
    
    if (!messageDiv) {
        messageDiv = document.createElement('div');
        messageDiv.id = 'create-account-message';
        messageDiv.className = 'form-message';
        
        // Вмъкваме го след формата
        const form = document.getElementById('createAccountForm');
        form.parentNode.insertBefore(messageDiv, form.nextSibling);
    }
    
    messageDiv.textContent = message;
    messageDiv.className = `form-message ${type}`;
    messageDiv.style.display = 'block';
    
    setTimeout(() => {
        messageDiv.style.display = 'none';
    }, 5000);
}


async function createAccount(ownerName, initialAmount) {
    try {
        const response = await fetch(`${API_BASE}/create_account`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                owner_name: ownerName,
                amount: parseFloat(initialAmount)
            })
        });

        const data = await response.json();
        console.log("Create response data:", data);

        return data;
    } catch (error) {
        console.error("Error during create account:", error);
        return { success: false, message: "Network error during creating account." };
    }
}


async function handleCreateAccount(e) {
    e.preventDefault();

    const ownerName = document.getElementById("owner_name").value.trim();
    const amount = document.getElementById("initial_amount").value;

    if (!ownerName || !amount || parseFloat(amount) < 0) {
        showMessage('Please enter a valid owner name and amount', 'error');
        return;
    }

    const submitBtn = e.target.querySelector('button[type="submit"]');
    const submitOriginalText = submitBtn.textContent;
    submitBtn.disabled = true;
    submitBtn.textContent = 'Processing...';

    const result = await createAccount(ownerName, amount);
    
    submitBtn.disabled = false;
    submitBtn.textContent = submitOriginalText;

    if (result && result.success) {
        document.getElementById('owner_name').value = '';
        document.getElementById('initial_amount').value = '';
        showMessage(`Success! Account created with ID: ${result.account_id}`, 'success');
        console.log('Success created account:', result);
    } else {
        showMessage(`Failed to create account: ${result.message || 'Unknown error'}`, 'error');
        console.log('Failed created account:', result);
    }
}


document.addEventListener('DOMContentLoaded', () => {
    const accountForm = document.getElementById('createAccountForm');
    accountForm.addEventListener('submit', handleCreateAccount);
    console.log('Event listeners for create account form set up');
});