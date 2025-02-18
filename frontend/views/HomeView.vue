<template>
  <div class="min-h-screen bg-gray-50 py-8">
    <div class="max-w-7xl mx-auto px-4">
      <h1 class="text-3xl font-bold text-center mb-8">区块链备份验证工具</h1>

      <!-- 错误提示 -->
      <el-alert v-if="errorMessage" :title="errorMessage" type="error" show-icon class="mb-4" />

      <div class="space-y-8">
        <!-- 文件上传区域 -->
        <div class="bg-white rounded-lg shadow p-6">
          <FileUpload @file-select="handleFileSelect" @file-remove="handleFileRemove" />
        </div>

        <!-- 验证选项区域 -->
        <div class="bg-white rounded-lg shadow p-6">
          <ValidationOptions :disabled="!fileId" @options-change="startValidation" />
        </div>

        <!-- 验证进度区域 -->
        <div class="bg-white rounded-lg shadow p-6">
          <Progress
            :progress="validationProgress.progress"
            :stage="validationProgress.stage"
            :verified-blocks="validationProgress.verifiedBlocks ?? 0"
            :is-paused="isPaused"
            @pause="handlePause"
            @resume="handleResume"
            @cancel="handleCancel"
          />
        </div>

        <!-- 验证结果区域 -->
        <div class="bg-white rounded-lg shadow p-6">
          <Results :result="validationResult" @export="setShowReport(true)" />
        </div>

        <!-- 报告区域 -->
        <div class="bg-white rounded-lg shadow p-6">
          <Reports
            v-if="showReport"
            :report="{
              metadata: {
                fileName: selectedFile?.name || '',
                validationDate: new Date().toISOString(),
                validationDuration: validationResult?.stats.timeSpent || 0,
                validatorVersion: '1.0.0',
              },
              result: validationResult!,
            }"
            @export="handleExport"
            @print="handlePrint"
            @share="handleShare"
          />
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { ElMessage } from 'element-plus'
import FileUpload from '@/components/FileUpload/FileUpload.vue'
import ValidationOptions from '@/components/ValidationOptions/index.vue'
import Progress from '@/components/Progress/index.vue'
import Results from '@/components/Results/index.vue'
import Reports from '@/components/Reports/index.vue'
import { apiService } from '@/api'
import {
  type ValidationOptions as IValidationOptions,
  type ValidationProgress,
  type ValidationResult,
} from '@/api/types'
import type { ReportFormat } from '@/components/Reports/types'

defineOptions({
  name: 'HomeView',
})

// 模拟数据
const selectedFile = ref<File | null>(null)
const fileId = ref('')
const isValidating = ref(true)
const validationProgress = ref<ValidationProgress>({
  stage: 'hash',
  progress: 60,
  verifiedBlocks: 100,
})
const isPaused = ref(false)
const validationResult = ref<ValidationResult>({
  hashVerification: [
    {
      status: 'success',
      blockId: 'block-1',
      hash: 'hash-value-1',
    },
    {
      status: 'error',
      blockId: 'block-2',
      hash: 'hash-value-2',
    },
  ],
  stats: {
    timeSpent: 1000,
  },
})
const showReport = ref(true)
const errorMessage = ref<string | null>(null)

// 文件处理
const handleFileSelect = async (file: File) => {
  try {
    const result = await apiService.uploadFile(file)
    selectedFile.value = file
    fileId.value = result.fileId
    errorMessage.value = null
    isValidating.value = false
    validationResult.value = {
      // 使用空对象替代 null
      hashVerification: [],
      stats: {
        timeSpent: 0,
      },
    }
    showReport.value = false
  } catch {
    errorMessage.value = '文件上传失败'
  }
}

const handleFileRemove = () => {
  selectedFile.value = new File([], 'test-backup.file')
  fileId.value = 'test-file-id'
  isValidating.value = true
  validationResult.value = {
    // 使用空对象替代 null
    hashVerification: [],
    stats: {
      timeSpent: 0,
    },
  }
  showReport.value = false
  errorMessage.value = null
}

// 验证控制
const startValidation = async (options: IValidationOptions) => {
  console.log('开始验证:', options)
  if (!fileId.value) return

  try {
    await apiService.startValidation(fileId.value, options)
    isValidating.value = true
    errorMessage.value = null
  } catch {
    errorMessage.value = '启动验证失败'
  }
}

const handlePause = async () => {
  if (!fileId.value) return
  isPaused.value = true
  try {
    await apiService.pauseValidation(fileId.value)
  } catch {
    errorMessage.value = '暂停验证失败'
  }
}

const handleResume = async () => {
  if (!fileId.value) return
  isPaused.value = false
  try {
    await apiService.resumeValidation(fileId.value)
  } catch {
    errorMessage.value = '恢复验证失败'
  }
}

const handleCancel = async () => {
  if (!fileId.value) return

  try {
    await apiService.cancelValidation(fileId.value)
    isValidating.value = false
    validationProgress.value = {
      stage: 'preparing',
      progress: 0,
      verifiedBlocks: 0,
    }
  } catch {
    errorMessage.value = '取消验证失败'
  }
}

// 报告处理
const handleExport = async (format: ReportFormat) => {
  if (!fileId.value) return

  try {
    const blob = await apiService.exportReport(fileId.value, format)
    const url = window.URL.createObjectURL(blob)
    const a = document.createElement('a')
    a.href = url
    a.download = `validation-report.${format}`
    document.body.appendChild(a)
    a.click()
    document.body.removeChild(a)
    window.URL.revokeObjectURL(url)
  } catch {
    errorMessage.value = '导出报告失败'
  }
}

const handlePrint = () => {
  window.print()
}

const handleShare = async () => {
  if (!fileId.value) return

  try {
    const shareUrl = await apiService.shareReport(fileId.value)
    await navigator.clipboard.writeText(shareUrl)
    ElMessage.success('分享链接已复制到剪贴板')
  } catch {
    errorMessage.value = '分享报告失败'
  }
}

const setShowReport = (value: boolean) => {
  showReport.value = value
}
</script>

<style scoped>
.space-y-8 > * + * {
  margin-top: 2rem;
}
</style>
