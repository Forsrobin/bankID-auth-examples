import type { UserData } from '@/lib/types/auth'
import React, { createContext, useContext, useState, type ReactNode } from 'react'

type UserContextType = {
  user: UserData | null
  setUser: React.Dispatch<React.SetStateAction<UserData | null>>
}

const UserContext = createContext<UserContextType | undefined>(undefined)

export const UserProvider = ({ children }: { children: ReactNode }) => {
  const [user, setUser] = useState<UserData | null>(null)
  return <UserContext.Provider value={{ user, setUser }}>{children}</UserContext.Provider>
}

export const useUserContext = () => {
  const context = useContext(UserContext)
  if (!context) {
    throw new Error('useUserContext must be used within a UserProvider')
  }
  return context
}
