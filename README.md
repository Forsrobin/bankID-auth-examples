<img src="./assets/images/bankid.png" alt="BankID Logo" width="120" />

# BankID Authentication Examples

This repository contains multiple examples of how to implement **BankID authentication** in different programming languages and frameworks. The goal is to provide simple, self-contained authentication flows following [BankID's official guidelines](https://www.bankid.com/rp-info) and best practices.

Each example includes:

- A basic authentication flow
- A simple JWT token issued after successful authentication
- A separate implementation per language/framework
- Full support for **BankID test environment**

---

## 🔧 Supported Implementations (Examples)

| Language / Framework | Path / Folder             |
| -------------------- | ------------------------- |
| ✅ Next.js           | `/examples/nextjs-bankid` |
| 🚧 React & Node.js   | _(coming soon)_           |
| 🚧 C++               | _(coming soon)_           |
| 🚧 Rust              | _(coming soon)_           |
| 🚧 Python            | _(coming soon)_           |
| 🚧 Go                | _(coming soon)_           |

---

## 📌 Key Features

- Compliant with BankID's technical requirements and flow (Auth → Collect → Completion).
- Designed for learning and quick prototyping.
- Stateless JWT-based session handling.
- Easy to extend into production systems.

---

## ⚠️ Important: Certificates & Security Notes

All examples in this repository use **BankID test certificates**. These are only valid for development and testing purposes in BankID’s test environment.

### If deploying to production:

- You **must** acquire production certificates from **BankID** via your integration partner or BankID directly.
- **Never commit** certificates or sensitive data to version control.
- Certificates should be securely loaded via environment variables or secure configuration tools.
- Make sure `.gitignore` includes all sensitive `.pem`, `.p12`, and `.crt` files used locally.

Example `.env` variables:

```env
BANKID_CLIENT_CERT=path/to/client.pem
BANKID_CLIENT_KEY=path/to/key.pem
BANKID_CA_CERT=path/to/ca.pem
```

---

## 📚 Resources

- [BankID Developer Documentation (in Swedish)](https://developers.bankid.com/)
- [BankID Test Environment](https://developers.bankid.com/test-portal/testing/identify)

---

## 📬 Contributing

Feel free to contribute with examples in more languages (e.g. Python, Go, Rust, C++) or improvements to existing ones. Please ensure your implementation matches the BankID flow and includes certificate management, proper error handling, and basic JWT session handling.

---

## 📝 License

This project is licensed under the MIT License.
