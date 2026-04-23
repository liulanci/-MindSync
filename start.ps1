$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  剪贴板同步器 - 启动脚本" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

$serverDir = Join-Path $projectRoot "server"
$clientDir = Join-Path $projectRoot "client"

Write-Host "[1/3] 检查服务端依赖..." -ForegroundColor Yellow
if (-not (Test-Path (Join-Path $serverDir "node_modules"))) {
    Write-Host "安装服务端依赖..." -ForegroundColor Yellow
    Push-Location $serverDir
    npm install
    Pop-Location
} else {
    Write-Host "服务端依赖已安装" -ForegroundColor Green
}

Write-Host ""
Write-Host "[2/3] 检查客户端依赖..." -ForegroundColor Yellow
if (-not (Test-Path (Join-Path $clientDir "node_modules"))) {
    Write-Host "安装客户端依赖..." -ForegroundColor Yellow
    Push-Location $clientDir
    npm install
    Pop-Location
} else {
    Write-Host "客户端依赖已安装" -ForegroundColor Green
}

Write-Host ""
Write-Host "[3/3] 启动服务..." -ForegroundColor Yellow
Write-Host ""
Write-Host "提示: 请确保MySQL服务已启动且配置正确" -ForegroundColor Red
Write-Host "提示: 首次运行请先执行: cd server && npm run init-db" -ForegroundColor Red
Write-Host ""

Write-Host "启动后端服务 (端口 3000)..." -ForegroundColor Green
Start-Process -FilePath "npm" -ArgumentList "run", "dev" -WorkingDirectory $serverDir -NoNewWindow

Start-Sleep -Seconds 3

Write-Host "启动桌面客户端..." -ForegroundColor Green
Start-Process -FilePath "npm" -ArgumentList "start" -WorkingDirectory $clientDir

Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  服务已启动!" -ForegroundColor Green
Write-Host "  后端API: http://localhost:3000" -ForegroundColor White
Write-Host "  健康检查: http://localhost:3000/api/health" -ForegroundColor White
Write-Host "=====================================" -ForegroundColor Cyan
