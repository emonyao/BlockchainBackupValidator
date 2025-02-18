<template>
  <div class="app-container">
    <img src="@/assets/07.gif" alt="background" class="background-animation" />

    <el-header height="80px">
      <div class="header-content">
        <div class="logo-container">
          <img src="@/assets/chinamobile.png" alt="中国移动" class="logo" />
        </div>
        <nav class="nav-menu">
          <router-link to="/">验证工具</router-link>
          <router-link to="/report">验证报告</router-link>
        </nav>
      </div>
    </el-header>

    <el-main class="main-content">
      <router-view></router-view>
    </el-main>
  </div>
</template>

<style scoped>
.app-container {
  min-height: 100vh;
  position: relative;
}

.background-animation {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  object-fit: cover;
  z-index: -1;
}

.header-content {
  max-width: 1120px;
  margin: 0 auto;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 40px;
  background-color: rgba(255, 255, 255, 0.95);
  backdrop-filter: blur(8px);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
  border-radius: 0 0 12px 12px;
}

.logo-container {
  height: 150px;
  width: 200px;
  padding: 10px 0;
}

.logo {
  height: 100%;
  width: auto;
  object-fit: contain;
  filter: drop-shadow(0 2px 4px rgba(0, 0, 0, 0.1));
}

.nav-menu {
  display: flex;
  gap: 40px;
}

.nav-menu a {
  color: #2c3e50;
  text-decoration: none;
  font-size: 18px;
  font-weight: 500;
  padding: 10px 20px;
  border-radius: 6px;
  transition: all 0.3s ease;
  position: relative;
}

.nav-menu a:hover {
  color: #40a9ff;
  background-color: rgba(240, 245, 255, 0.8);
  transform: translateY(-2px);
}

.nav-menu a.router-link-active {
  color: #40a9ff;
  background-color: rgba(240, 245, 255, 0.8);
}

.nav-menu a::after {
  content: '';
  position: absolute;
  bottom: 0;
  left: 50%;
  width: 0;
  height: 2px;
  background-color: #40a9ff;
  transition: all 0.3s ease;
  transform: translateX(-50%);
}

.nav-menu a:hover::after,
.nav-menu a.router-link-active::after {
  width: 80%;
}

.main-content {
  max-width: 1200px;
  margin: 30px auto;
  padding: 40px;
  background-color: rgba(255, 255, 255, 0.98);
  backdrop-filter: blur(8px);
  border-radius: 16px;
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.15);
}
</style>
