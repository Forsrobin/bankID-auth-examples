import { useEffect, useRef, useState } from 'react'

export function useCountdown(start: number, onExpire: () => void) {
  const [countdown, setCountdown] = useState(start)
  const intervalRef = useRef<number | null>(null)

  useEffect(() => {
    if (start > 0) setCountdown(start)
  }, [start])

  useEffect(() => {
    if (countdown > 0) {
      intervalRef.current = setInterval(() => {
        setCountdown((prev) => {
          if (prev <= 1) {
            clearInterval(intervalRef.current!)
            onExpire()
            return 0
          }
          return prev - 1
        })
      }, 1000)
    }

    return () => clearInterval(intervalRef.current!)
  }, [countdown, onExpire])

  return countdown
}
