import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  root: '.',
  publicDir: 'public',
  build: {
    outDir: 'dist',
    emptyOutDir: true,
  },
  css: {
    modules: false,
  },
  server: {
    port: 1234, // matching the old parcel default
    open: true,
    watch: {
      usePolling: true,
      interval: 1000,
    },
  },
  resolve: {
    alias: {
      // This helps Vite find the compiled ReasonML files
      '@': '/src',
    },
  },
}) 