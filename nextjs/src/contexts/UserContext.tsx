'use client'
import type { CompletionDataV6 } from 'bankid'
import React, { createContext, useContext, useState, type ReactNode } from 'react'

type UserContextType = {
  user: CompletionDataV6['user'] | null
  setUser: React.Dispatch<React.SetStateAction<CompletionDataV6['user'] | null>>
}

const UserContext = createContext<UserContextType | undefined>(undefined)

export const UserProvider = ({ children }: { children: ReactNode }) => {
  const [user, setUser] = useState<CompletionDataV6['user'] | null>(null)
  return <UserContext.Provider value={{ user, setUser }}>{children}</UserContext.Provider>
}

export const useUserContext = () => {
  const context = useContext(UserContext)
  if (!context) {
    throw new Error('useUserContext must be used within a UserProvider')
  }
  return context
}
