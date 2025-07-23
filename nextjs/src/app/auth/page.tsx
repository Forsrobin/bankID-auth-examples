'use client'
import BankIdLogo from '@/assets/images/bankid.png'
import { BankIDModal } from '@/components/BankIdModal'
import { Button } from '@/components/ui/button'
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card'
import { useCountdown } from '@/hooks/useTimeout'
import { useAuthStateMachine } from '@/hooks/useAuthStateMachine'
import { useMutation, useQuery } from '@tanstack/react-query'
import { Shield } from 'lucide-react'
import Image from 'next/image'
import { useState, useEffect } from 'react'
import type { InitAuthResponse } from '../api/auth/init/route'
import type { PoolAuthResponse } from '../api/auth/poll/route'

export default function Auth() {
  const [showBankIDModal, setShowBankIDModal] = useState(false)
  const { state, actions } = useAuthStateMachine()

  // Track the countdown timer
  const countdown = useCountdown(state.authCountdown, () => {
    actions.reset() // This will set status to 'failed' and clear states
  })

  // Poll auth status
  const { data: pollAuthData } = useQuery({
    queryKey: ['pollAuth', state.orderRef],
    queryFn: async () => {
      const res = await fetch(`/api/auth/poll?orderRef=${state.orderRef}`)
      if (!res.ok) throw new Error('Failed to poll auth status')
      return (await res.json()) as PoolAuthResponse
    },
    enabled: state.orderRef !== null,
    refetchInterval: 1000,
    refetchOnWindowFocus: false,
  })

  // Initialize auth
  const authMutation = useMutation({
    mutationFn: async () => {
      await fetch(`/api/auth/poll`) // This is just to trigger the initial state
      const res = await fetch('/api/auth/init')
      if (!res.ok) throw new Error('Failed to init auth')
      return (await res.json()) as InitAuthResponse
    },
    onSuccess: (data) => {
      actions.initAuth(data.orderRef, data.authCountdown)
    },
  })

  // Handle poll data changes
  useEffect(() => {
    if (pollAuthData) {
      actions.handlePollData(pollAuthData)
    }
  }, [pollAuthData])

  const handleBankIDLogin = () => {
    setShowBankIDModal(true)
    actions.reset()
    authMutation.mutate()
  }

  const retryLogin = () => {
    authMutation.reset()
    handleBankIDLogin()
  }

  const cancelPolling = () => {
    actions.reset()
    setShowBankIDModal(false)
    authMutation.reset()
  }

  return (
    <div className='min-h-screen flex flex-col bg-gradient-to-br from-blue-50 via-white to-blue-50'>
      <div className='container flex-1 mx-auto px-4 py-12 flex items-center justify-center pb-32'>
        <div className='mx-auto max-w-6xl space-y-6'>
          <div className={`h-28 w-full relative`}>
            <Image src={BankIdLogo} alt='BankID Logo' className='object-contain' fill />
          </div>
          <div className='grid gap-12 lg:grid-cols-1 lg:gap-16'>
            <div className='space-y-4'>
              <h2 className='text-4xl font-bold justify-center flex items-center gap-2 tracking-tight text-gray-900'>
                <span className='block text-primary'>BankID</span>
                Login
              </h2>
              <p className='text-lg text-center text-gray-600'>Simple login demo using BankID and Next.js</p>
            </div>

            <div className='flex items-center justify-center'>
              <Card className='w-full max-w-md shadow-xl'>
                <CardHeader className='space-y-1 text-center'>
                  <CardTitle className='text-2xl font-bold'>Login</CardTitle>
                  <CardDescription>Open the BankID app to log in</CardDescription>
                </CardHeader>
                <CardContent className='space-y-6'>
                  <div className='space-y-4'>
                    <Button isLoading={showBankIDModal} onClick={handleBankIDLogin} className='w-full bg-primary hover:bg-primary-/90' size='lg'>
                      <Shield className='mr-2 h-5 w-5' />
                      Login with BankID
                    </Button>

                    <div className='relative'>
                      <div className='absolute inset-0 flex items-center'>
                        <span className='w-full border-t' />
                      </div>
                      <div className='relative flex justify-center text-xs uppercase'>
                        <span className='bg-white px-2 text-gray-500'>BankID</span>
                      </div>
                    </div>

                    <div className='rounded-lg bg-blue-50 p-4'>
                      <div className='flex items-start space-x-3'>
                        <div className='text-sm space-y-2'>
                          <div className={`flex items-center gap-1`}>
                            <Shield size={16} className='text-primary mt-0.5' />
                            <p className='font-medium text-primary'>Test login</p>
                          </div>
                          <p className='text-primary/80'>
                            This demo is using BankID test certificates and to log in with BankID you must have a test BankID account set up. To
                            create a test account, read more here:
                          </p>
                          <a
                            href='https://developers.bankid.com/test-portal/bankid-for-test'
                            target='_blank'
                            rel='noopener noreferrer'
                            className='font-medium text-primary underline'
                          >
                            Set up test account
                          </a>
                        </div>
                      </div>
                    </div>
                  </div>
                </CardContent>
              </Card>
            </div>
          </div>
        </div>
      </div>

      <BankIDModal
        setAuthState={(state) => {
          if (state === 'failed') actions.reset()
        }}
        authState={state.status}
        isOpen={showBankIDModal}
        cancelPolling={cancelPolling}
        qrCode={state.qrCode}
        authCountdown={countdown}
        retryLogin={retryLogin}
      />
    </div>
  )
}
