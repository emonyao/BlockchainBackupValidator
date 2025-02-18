<template>
  <div class="progress-container">
    <el-progress :percentage="progress" :status="status" />
    <div class="progress-info">
      <span>当前验证阶段: {{ currentStage }}</span>
      <span>已验证区块: {{ verifiedBlocks }}</span>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'

const props = defineProps<{
  stage: string
  progress: number
  verifiedBlocks: number
}>()

const currentStage = computed(() => {
  const stages = {
    hash: '哈希验证',
    link: '链接验证',
    account: '账户验证',
  }
  return stages[props.stage as keyof typeof stages] || '未知阶段'
})

const status = computed(() => {
  if (props.progress === 100) return 'success'
  return 'normal'
})
</script>
