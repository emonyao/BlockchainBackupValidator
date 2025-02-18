import { defineStore } from 'pinia'

export const useValidationStore = defineStore('validation', {
  state: () => ({
    currentFile: null as File | null,
    validationStatus: 'idle',
    progress: 0
  }),
  actions: {
    updateProgress(progress: number) {
      this.progress = progress
    }
  }
}
