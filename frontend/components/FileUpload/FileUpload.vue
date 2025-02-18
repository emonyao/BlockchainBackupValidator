<template>
  <el-upload
    class="upload-container"
    drag
    action="/api/upload"
    :on-success="handleSuccess"
    :on-error="handleError"
    :before-upload="beforeUpload"
  >
    <i class="el-icon-upload"></i>
    <div class="el-upload__text">拖拽文件到此处或 <em>点击上传</em></div>
  </el-upload>
</template>

<script setup lang="ts">
import { ElMessage, ElUpload } from 'element-plus'
import 'element-plus/dist/index.css'
interface UploadResponse {
  success: boolean
  message: string
  fileId?: string
}
const handleSuccess = (response: UploadResponse) => {
  console.log('File uploaded:', response)
  console.log(response)
}

const handleError = (error: Error) => {
  console.error('Upload error:', error)
}

const beforeUpload = (file: File) => {
  const isValidSize = file.size / 1024 / 1024 < 100
  if (!isValidSize) {
    ElMessage.error('文件大小不能超过100MB')
  }
  return isValidSize
}
</script>
