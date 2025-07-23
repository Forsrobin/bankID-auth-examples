'use client'
import BankIdImage from '@/assets/images/bankid.png'
import { Button } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Dialog, DialogContent, DialogHeader, DialogTitle } from '@/components/ui/dialog'
import { AnimatePresence, motion } from 'framer-motion'
import { CheckCircle, Expand, Loader2, RefreshCw, Shrink, X } from 'lucide-react'
import Image from 'next/image'
import QRCode from 'react-qr-code'
import { useEffect, useRef, useState } from 'react'
import type { AuthState } from '@/lib/types/auth'
import { Spinner } from './ui/spinner'

interface BankIDModalProps {
  isOpen: boolean
  cancelPolling: () => void
  qrCode: string | null
  authState: AuthState
  setAuthState: (state: AuthState) => void
  retryLogin: () => void
  authCountdown: number
}

export const BankIDModal = ({ isOpen, cancelPolling, qrCode, authState, setAuthState, retryLogin, authCountdown }: BankIDModalProps) => {
  const [isFullscreen, setIsFullscreen] = useState(false)
  const [showExtendButton, setShowExtendButton] = useState(false)
  const [screenReaderCountdown, setScreenReaderCountdown] = useState('')
  const qrButtonRef = useRef<HTMLButtonElement>(null)
  const extendButtonRef = useRef<HTMLButtonElement>(null)
  const mainMessageRef = useRef<HTMLDivElement>(null)

  useEffect(() => {
    if (authState === 'qr-code' && authCountdown > 0) {
      setShowExtendButton(authCountdown <= 30)

      const minutes = Math.floor(authCountdown / 60)
      const seconds = authCountdown % 60

      if (minutes > 0 && seconds === 0) {
        setScreenReaderCountdown(`${minutes} minut${minutes !== 1 ? 's' : ''} left`)
      } else if (minutes === 0 && [50, 40, 30, 20, 10, 5, 4, 3, 2, 1].includes(seconds)) {
        setScreenReaderCountdown(`${seconds} second${seconds !== 1 ? 's' : ''} left`)
      }
    } else {
      setShowExtendButton(false)
      setScreenReaderCountdown('')
    }
  }, [authCountdown, authState])

  // Handle keyboard shortcuts
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (!isOpen || authState !== 'qr-code') return

      if (event.code === 'Space' && showExtendButton) {
        event.preventDefault()
        handleExtendTime()
      }

      if (event.code === 'Escape' && isFullscreen) {
        event.preventDefault()
        exitFullscreen()
      }
    }

    document.addEventListener('keydown', handleKeyDown)
    return () => document.removeEventListener('keydown', handleKeyDown)
  }, [isOpen, authState, showExtendButton, isFullscreen])

  useEffect(() => {
    if (isOpen && mainMessageRef.current) {
      setTimeout(() => {
        mainMessageRef.current?.focus()
      }, 100)
    }
  }, [authState, isOpen])

  const handleCancel = () => {
    setIsFullscreen(false)
    cancelPolling()
  }

  const handleRetry = () => {
    setIsFullscreen(false)
    retryLogin()
  }

  const handleQrClick = () => {
    if (isFullscreen) {
      exitFullscreen()
    } else {
      enterFullscreen()
    }
  }

  const enterFullscreen = () => {
    setIsFullscreen(true)
    setTimeout(() => {
      if (showExtendButton && extendButtonRef.current) {
        extendButtonRef.current.focus()
      } else if (qrButtonRef.current) {
        qrButtonRef.current.focus()
      }
    }, 100)
  }

  const exitFullscreen = () => {
    setIsFullscreen(false)
    setTimeout(() => {
      if (showExtendButton && extendButtonRef.current) {
        extendButtonRef.current.focus()
      } else if (qrButtonRef.current) {
        qrButtonRef.current.focus()
      }
    }, 100)
  }

  const handleExtendTime = () => {
    retryLogin()
    setTimeout(() => {
      qrButtonRef.current?.focus()
    }, 100)
  }

  const formatCountdownDisplay = (countdown: number) => {
    const minutes = Math.floor(countdown / 60)
    const seconds = countdown % 60
    if (minutes > 0) {
      return `${minutes}:${seconds.toString().padStart(2, '0')}`
    }
    return `${seconds}s`
  }

  return (
    <>
      <Dialog open={isOpen && !isFullscreen} onOpenChange={handleCancel}>
        <DialogContent className='sm:max-w-md'>
          <DialogHeader>
            <DialogTitle className='flex items-center space-x-4'>
              <Image src={BankIdImage} alt='BankID Logo' width={100} height={100} className='h-8 w-auto' />
              <span>BankID Authentication</span>
            </DialogTitle>
          </DialogHeader>

          <AnimatePresence mode='wait'>
            <motion.div
              key={authState}
              initial={{ opacity: 0, height: 0 }}
              animate={{ opacity: 1, height: 'auto' }}
              exit={{ opacity: 0, height: 0 }}
              transition={{ duration: 0.2, ease: 'easeInOut' }} // Use layout transition for smooth height changes
              layout
              className='space-y-6 overflow-hidden'
            >
              {authState === 'loading' && (
                <div className='text-center space-y-4'>
                  <div className='flex justify-center'>
                    <Spinner variant='ring' className='h-12 w-12 text-primary' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold'>Starting BankID...</h3>
                    <p className='text-sm text-gray-600'>Preparing secure connection</p>
                  </div>
                </div>
              )}

              {authState === 'qr-code' && qrCode && (
                <div className='space-y-4'>
                  <div className='text-center' ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold mb-2'>Identify yourself with BankID</h3>
                    <p className='text-sm text-gray-600 mb-4'>Open the BankID app on your mobile and scan the QR code below</p>
                  </div>

                  <Card className='bg-gray-50'>
                    <CardContent className='p-6'>
                      <div className='flex justify-center mb-4'>
                        <button
                          ref={qrButtonRef}
                          onClick={handleQrClick}
                          className='focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2 rounded-lg'
                          aria-label='QR code for BankID authentication. Click to open in fullscreen'
                          type='button'
                        >
                          <QRCode value={qrCode} size={200} bgColor='transparent' fgColor='black' level='H' />
                        </button>
                      </div>

                      <div className='text-center space-y-2'>
                        <div className='flex items-center justify-center space-x-2 text-sm text-gray-600'>
                          <Loader2 className='h-4 w-4 animate-spin' />
                          <span>Waiting for authentication...</span>
                        </div>

                        {authCountdown > 0 && (
                          <p className='text-sm font-medium text-muted-foreground' aria-hidden='true'>
                            Tid kvar: {formatCountdownDisplay(authCountdown)}
                          </p>
                        )}

                        <div aria-live='polite' aria-atomic='true' className='sr-only'>
                          {screenReaderCountdown}
                        </div>
                      </div>
                    </CardContent>
                  </Card>

                  {showExtendButton && (
                    <div className='flex justify-center'>
                      <Button ref={extendButtonRef} onClick={handleExtendTime} variant='outline' className='font-medium'>
                        <RefreshCw className='mr-2 h-4 w-4' />
                        Extend Time
                      </Button>
                    </div>
                  )}

                  <div className='bg-blue-50 p-4 rounded-lg'>
                    <h4 className='font-medium text-blue-900 mb-2'>Instructions:</h4>
                    <ol className='text-sm text-blue-800 space-y-1'>
                      <li>1. Open the BankID app on your mobile</li>
                      <li>2. Select "Scan QR code"</li>
                      <li>3. Point the camera at the QR code above</li>
                      <li>4. Follow the instructions in the app</li>
                    </ol>
                    <p className='text-xs text-blue-700 mt-2'>Tips: Click the QR code to open it in fullscreen</p>
                  </div>
                </div>
              )}

              {authState === 'waiting' && (
                <div className='text-center space-y-4 py-10'>
                  <div className='flex justify-center'>
                    <Loader2 className='h-12 w-12 animate-spin text-green-600' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold text-green-700'>Authentication in progress...</h3>
                    <p className='text-sm text-gray-600'>Complete the process in the BankID app</p>
                  </div>
                </div>
              )}

              {authState === 'success' && (
                <div className='text-center space-y-4 py-10'>
                  <div className='flex justify-center'>
                    <CheckCircle className='h-12 w-12 text-green-600' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold text-green-600'>Authentication successful!</h3>
                    <p className='text-sm text-gray-600'>You are being redirected to your profile...</p>
                  </div>
                </div>
              )}

              {authState === 'failed' && (
                <div className='text-center space-y-4 py-10'>
                  <div className='flex justify-center'>
                    <X className='h-12 w-12 text-red-700' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold text-red-700'>Failed to authenticate</h3>
                    <p className='text-sm text-gray-600'>{authCountdown === 0 ? 'Time expired.' : 'Something went wrong.'} Please try again.</p>
                  </div>
                  <Button className='w-full' onClick={handleRetry}>
                    <RefreshCw className='mr-2 h-4 w-4' />
                    Try again
                  </Button>
                </div>
              )}

              {(authState === 'qr-code' || authState === 'failed') && (
                <div className='flex justify-center'>
                  <Button variant='outline' onClick={handleCancel}>
                    Cancel
                  </Button>
                </div>
              )}
            </motion.div>
          </AnimatePresence>
        </DialogContent>
      </Dialog>

      {isFullscreen && authState === 'qr-code' && qrCode && (
        <div className='fixed inset-0 z-50 bg-white flex items-center justify-center p-4 overflow-auto'>
          <div className='w-full max-w-lg mx-auto text-center space-y-6'>
            <div className='flex items-center justify-between mb-6'>
              <h2 className='text-xl font-semibold'>BankID QR-Code</h2>
              <Button variant='ghost' size='sm' onClick={exitFullscreen} aria-label='Stäng fullskärm'>
                <Shrink className='h-4 w-4' />
              </Button>
            </div>

            <div className='bg-white border-2 border-gray-200 rounded-lg p-8 inline-block'>
              <button
                ref={qrButtonRef}
                onClick={exitFullscreen}
                className='focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2 rounded-lg'
                aria-label='QR-Code for BankID authentication. Click to exit fullscreen'
                type='button'
              >
                <QRCode value={qrCode} size={Math.min(300, window.innerWidth * 0.6)} bgColor='white' fgColor='black' level='H' />
              </button>
            </div>

            <div className='space-y-4'>
              {authCountdown > 0 && (
                <p className='text-lg font-medium' aria-hidden='true'>
                  Time left: {formatCountdownDisplay(authCountdown)}
                </p>
              )}

              <div aria-live='polite' aria-atomic='true' className='sr-only'>
                {screenReaderCountdown}
              </div>

              {showExtendButton && (
                <Button ref={extendButtonRef} onClick={handleExtendTime} size='lg' className='font-medium'>
                  <RefreshCw className='mr-2 h-5 w-5' />
                  Extend time
                </Button>
              )}
            </div>

            <div className='text-sm text-gray-600 max-w-md mx-auto'>
              <p>Scan the QR-Code with the BankID app on your mobile</p>
              <p className='mt-2'>Press space to extend the time when the button appears</p>
            </div>
          </div>
        </div>
      )}
    </>
  )
}
