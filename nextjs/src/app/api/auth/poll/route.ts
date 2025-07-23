import authService from '@/server/actions/auth/auth.service'
import type { CompletionDataV6 } from 'bankid'
import { NextResponse } from 'next/server'

export const runtime = 'nodejs'

export type PoolAuthResponse = {
  status: 'qrCode' | 'newOrderRef' | 'complete' | 'failed'
  qrCode: string | null
  orderRef: string | null
  token: string | null
  user: CompletionDataV6['user'] | null
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
