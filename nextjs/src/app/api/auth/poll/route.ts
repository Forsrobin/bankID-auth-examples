import authService from '@/server/actions/auth/auth.service'
import { NextResponse } from 'next/server'

export type PoolAuthResponse = {
  status: 'qrCode' | 'newOrderRef' | 'complete' | 'failed'
  qrCode: string | null
  orderRef: string | null
  token: string | null
}

export async function GET(request: Request) {
  const url = new URL(request.url)
  const orderRef = url.searchParams.get('orderRef')

  if (!orderRef) {
    return NextResponse.json({ error: 'orderRef is required' }, { status: 400 })
  }
  const result = await authService.pollAuth(orderRef)
  return NextResponse.json(result)
}
