export type AuthState = 'pending' | 'loading' | 'qr-code' | 'waiting' | 'success' | 'failed'

export type UserData = {
  personalNumber: string
  givenName: string
  surname: string
  name: string
}

export type PoolAuthResponse = {
  status: 'qrCode' | 'newOrderRef' | 'complete' | 'failed'
  qrCode: string | null
  orderRef: string | null
  token: string | null
  user: UserData | null
}

export type AuthInitResponse = {
  orderRef: string
  authCountdown: number
  autoStartToken: string
}
