import { useUserContext } from '@/contexts/UserContext'
import type { AuthState, PoolAuthResponse } from '@/lib/types/auth'
import { useRouter } from '@tanstack/react-router'
import { useReducer } from 'react'
import { Cookies } from 'react-cookie'

interface AuthStateMachineState {
  status: AuthState
  qrCode: string | null
  orderRef: string | null
  authCountdown: number
}

const cookies = new Cookies()

type AuthAction =
  | { type: 'INIT_AUTH'; payload: { orderRef: string; authCountdown: number } }
  | { type: 'SET_QR_CODE'; payload: { qrCode: string; orderRef?: string } }
  | { type: 'AUTH_SUCCESS'; payload: { token: string; user: any } }
  | { type: 'AUTH_FAILED' }
  | { type: 'RESET' }

const initialState: AuthStateMachineState = {
  status: 'loading',
  qrCode: null,
  orderRef: null,
  authCountdown: 0,
}

function authReducer(state: AuthStateMachineState, action: AuthAction): AuthStateMachineState {
  switch (action.type) {
    case 'INIT_AUTH':
      return {
        ...state,
        status: 'loading',
        orderRef: action.payload.orderRef,
        authCountdown: action.payload.authCountdown,
      }

    case 'SET_QR_CODE':
      return {
        ...state,
        status: 'qr-code',
        qrCode: action.payload.qrCode,
        orderRef: action.payload.orderRef || state.orderRef,
      }

    case 'AUTH_SUCCESS':
      return {
        ...state,
        status: 'success',
        qrCode: null,
        orderRef: null,
        authCountdown: 0,
      }

    case 'AUTH_FAILED':
      return {
        ...state,
        status: 'failed',
        qrCode: null,
        orderRef: null,
        authCountdown: 0,
      }

    case 'RESET':
      return initialState

    default:
      return state
  }
}

export function useAuthStateMachine() {
  const [state, dispatch] = useReducer(authReducer, initialState)
  const { setUser } = useUserContext()
  const router = useRouter()

  // Handle poll data changes
  const handlePollData = (pollData: PoolAuthResponse) => {
    switch (pollData.status) {
      case 'newOrderRef':
        dispatch({
          type: 'SET_QR_CODE',
          payload: {
            qrCode: pollData.qrCode!,
            orderRef: pollData.orderRef!,
          },
        })
        break

      case 'qrCode':
        dispatch({
          type: 'SET_QR_CODE',
          payload: { qrCode: pollData.qrCode! },
        })
        break

      case 'failed':
        dispatch({ type: 'AUTH_FAILED' })
        break

      case 'complete':
        // Create a access token and set it in cookies
        // WARNING: This is a simplified example, in production you should handle this securely
        // using refresh tokens. In a real application, you would also want to verify the token
        // and handle errors appropriately and store the user data securely.
        cookies.set('accessToken', pollData.token, {
          maxAge: 60 * 60 * 24 * 30,
          expires: new Date(Date.now() + 60 * 60 * 24 * 30 * 1000),
          sameSite: 'lax',
          secure: true,
        })

        setUser(pollData.user)
        dispatch({
          type: 'AUTH_SUCCESS',
          payload: { token: pollData.token!, user: pollData.user! },
        })

        setTimeout(() => router.navigate({ to: '/' }), 300)
        break
    }
  }

  const initAuth = (orderRef: string, authCountdown: number) => {
    dispatch({
      type: 'INIT_AUTH',
      payload: { orderRef, authCountdown },
    })
  }

  const reset = () => {
    dispatch({ type: 'RESET' })
  }

  return {
    state,
    actions: {
      handlePollData,
      initAuth,
      reset,
    },
  }
}
