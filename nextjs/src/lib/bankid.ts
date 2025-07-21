import { BankIdClientV6 } from 'bankid'
import env from '../server/env'

export const AUTH_TIMEOUT = 300

export const bankIdClient = new BankIdClientV6({
  production: env.NODE_ENV === 'production',
  qrOptions: {
    orderTTL: AUTH_TIMEOUT,
  },
  refreshInterval: 1000,
  qrEnabled: true,
  passphrase: 'qwerty123',
  ca: 'src/certs/test.ca',
  pfx: 'src/certs/FPTestcert5_20240610.p12',
})

export default bankIdClient
