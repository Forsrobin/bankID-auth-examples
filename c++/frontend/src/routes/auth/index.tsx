import BankIdLogo from '@/assets/images/bankid.png'
import { BankIDModal } from '@/components/BankIdModal'
import { Button } from '@/components/ui/button'
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card'
import { useAuthStateMachine } from '@/hooks/useAuthStateMachine'
import { useCountdown } from '@/hooks/useTimeout'
import type { AuthInitResponse } from '@/lib/types/auth'
import { useMutation, useQuery } from '@tanstack/react-query'
import { createFileRoute } from '@tanstack/react-router'
import { Shield } from 'lucide-react'
import { useEffect, useState } from 'react'

export const Route = createFileRoute('/auth/')({
  component: Auth,
})

export default function Auth() {
  const [showBankIDModal, setShowBankIDModal] = useState(false)
  const { state, actions } = useAuthStateMachine()
  const baseApiUrl = 'http://localhost:8080'
  console.log(baseApiUrl)

  // Track the countdown timer
  const countdown = useCountdown(state.authCountdown, () => {
    actions.reset() // This will set status to 'failed' and clear states
  })

  // Poll auth status
  const { data: pollAuthData } = useQuery({
    queryKey: ['pollAuth', state.orderRef],
    queryFn: async () => {
      const res = await fetch(`${baseApiUrl}/api/auth/poll/${state.orderRef}`)
      if (!res.ok) throw new Error('Failed to poll auth status')
      return await res.json()
    },
    enabled: state.orderRef !== null,
    refetchInterval: 1000,
    refetchOnWindowFocus: false,
  })

  // Initialize auth
  const authMutation = useMutation({
    mutationFn: async () => {
      const res = await fetch(`${baseApiUrl}/api/auth/init`)
      if (!res.ok) throw new Error('Failed to init auth')
      return (await res.json()) as AuthInitResponse
    },
    onSuccess: (data) => {
      actions.initAuth(data.orderRef, data.authCountdown)
    },
  })

  // Cancel bankID request
  const cancelBankIDRequest = useMutation({
    mutationFn: async () => {
      const res = await fetch(`${baseApiUrl}/api/auth/cancel/${state.orderRef}`, {
        method: 'GET',
      })
      if (!res.ok) throw new Error('Failed to cancel auth')
      return await res.json()
    },
    onSuccess: () => {
      cancelPolling()
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
            <img src={BankIdLogo} width={120} alt='BankID Logo' className='mx-auto object-contain' />
          </div>
          <div className='grid gap-12 lg:grid-cols-1 lg:gap-16'>
            <div className='space-y-4'>
              <h2 className='text-4xl font-bold justify-center flex items-center gap-2 tracking-tight text-gray-900'>
                <span className='block text-primary'>BankID</span>
                Login
              </h2>
              <p className='text-lg text-center text-gray-600'>Simple BankID login demo using React with TanStack Router and c++ (Crowcpp) backend</p>
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
