<p align="center">
  <img src="./public/bankid.png" alt="BankID Logo" width="200" height="200" />
</p>

# BankID Authentication Example - Next.js


This is a [Next.js](https://nextjs.org) project demonstrating BankID QR code authentication implementation. The project showcases how to integrate Swedish BankID authentication into a modern web application using Next.js 14 with the App Router.

## Features

- 🔐 BankID QR Code Authentication
- 🛡️ JWT-based session management
- 🚀 Next.js 14 with App Router
- ⚡ Real-time authentication status updates
- 🎨 Modern UI with Tailwind CSS and shadcn/ui components

## Installation and Setup

### Prerequisites

- Node.js 18+
- Yarn package manager

### Installation

1. Clone the repository and navigate to the nextjs folder:

```bash
cd nextjs
```

2. Install dependencies using yarn:

```bash
yarn install
```

3. Create environment variables (see [Environment Variables](#environment-variables) section)

4. Set up certificates (see [Certificates](#certificates) section)

### Running the Application

Start the development server using yarn:

```bash
yarn dev
```

Open [http://localhost:3000](http://localhost:3000) with your browser to see the result.


### Testing the Demo


> [!WARNING]
> **Important**: To test the local demo, you will need a **BankID test account**.


This demo uses BankID test certificates, which means you cannot log in with a regular BankID account. You must have a test BankID account set up to authenticate successfully.

**To create a test account:**

1. Visit the [BankID Test Portal](https://developers.bankid.com/test-portal/bankid-for-test)
2. Follow the instructions to set up your test BankID account
3. Use this test account when testing the authentication flow in the demo

Without a test account, the authentication will fail even if everything is configured correctly.

## Environment Variables

Create a `.env.local` file in the root of the nextjs project with the following variables:

```env
NODE_ENV=development
JWT_SECRET=your-super-secret-jwt-key-here
```

**Required Variables:**

- `NODE_ENV`: Set to `development` for local development or `production` for production
- `JWT_SECRET`: A secure secret key for JWT token signing and verification

These variables are defined and validated in `src/server/env.ts`.

## Certificates

> [!WARNING]
> **Important Security Notice**: Certificate files should **ALWAYS** be added to `.gitignore` and never committed to version control.

This demo uses BankID test certificates located in `src/certs/`:

- `FPTestcert5_20240610.p12` - Test certificate for BankID integration
- `test.ca` - Test Certificate Authority
- `prod.ca` - Production Certificate Authority (for production use)

For production deployments, you'll need to:

1. Obtain production certificates from your BankID provider
2. Store them securely (environment variables, secure file storage, etc.)
3. Update the certificate paths in `src/lib/bankid.ts`

## Authentication & Middleware

The application uses a custom middleware (`src/middleware.ts`) that:

- Validates JWT access tokens on protected routes
- Automatically redirects unauthenticated users to `/auth`
- Clears invalid/expired tokens
- Prevents authenticated users from accessing the auth page

The middleware runs on the root path (`/`) and ensures only authenticated users can access the main application.

## BankID Integration

This demo specifically demonstrates **BankID QR Code authentication**. The BankID client is configured in `src/lib/bankid.ts` using the [`bankid`](https://www.npmjs.com/package/bankid) npm package.

### Available BankID Services

While this demo focuses on QR code authentication, the BankID API package provides access to all BankID services including:

- QR Code authentication (implemented in this demo)
- Same device authentication
- Mobile authentication
- Signing services
- Identity verification

For more information about additional BankID services and implementation details, visit the [BankID package GitHub repository](https://github.com/ansurudeen/bankid).

## Project Structure

```
src/
├── app/                    # Next.js App Router pages
├── components/             # React components
├── lib/                    # Utility libraries and BankID config
├── server/                 # Server-side utilities and environment config
├── middleware.ts           # Authentication middleware
└── certs/                  # BankID certificates (gitignored)
```

## Learn More

To learn more about the technologies used in this project:

- [Next.js Documentation](https://nextjs.org/docs) - Learn about Next.js features and API
- [BankID API Documentation](https://www.npmjs.com/package/bankid) - Learn about BankID integration
- [JWT Authentication](https://jwt.io/introduction) - Learn about JSON Web Tokens

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is open source and available under the [MIT License](../LICENSE).
