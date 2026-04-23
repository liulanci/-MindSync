const authService = require('../services/authService');
const { v4: uuidv4 } = require('uuid');

describe('Auth Service', () => {
  describe('register', () => {
    it('should register a new user successfully', async () => {
      const userData = {
        username: 'testuser',
        email: 'test@example.com',
        password: 'Test123!',
      };

      const result = await authService.register(
        userData.username,
        userData.email,
        userData.password
      );

      expect(result).toHaveProperty('userId');
      expect(result).toHaveProperty('username', userData.username);
      expect(result).toHaveProperty('email', userData.email);
      expect(result).toHaveProperty('token');
    });

    it('should throw error for duplicate username', async () => {
      const userData = {
        username: 'duplicate',
        email: 'test1@example.com',
        password: 'Test123!',
      };

      await authService.register(userData.username, userData.email, userData.password);

      await expect(
        authService.register(userData.username, 'test2@example.com', 'Test123!')
      ).rejects.toThrow('用户名或邮箱已存在');
    });

    it('should throw error for duplicate email', async () => {
      const userData = {
        username: 'user1',
        email: 'duplicate@example.com',
        password: 'Test123!',
      };

      await authService.register(userData.username, userData.email, userData.password);

      await expect(
        authService.register('user2', userData.email, 'Test123!')
      ).rejects.toThrow('用户名或邮箱已存在');
    });
  });

  describe('login', () => {
    beforeEach(async () => {
      await authService.register('loginuser', 'login@example.com', 'Test123!');
    });

    it('should login successfully with correct credentials', async () => {
      const result = await authService.login('login@example.com', 'Test123!');

      expect(result).toHaveProperty('userId');
      expect(result).toHaveProperty('username', 'loginuser');
      expect(result).toHaveProperty('email', 'login@example.com');
      expect(result).toHaveProperty('token');
    });

    it('should throw error for incorrect password', async () => {
      await expect(
        authService.login('login@example.com', 'WrongPassword!')
      ).rejects.toThrow('邮箱或密码错误');
    });

    it('should throw error for non-existent email', async () => {
      await expect(
        authService.login('nonexistent@example.com', 'Test123!')
      ).rejects.toThrow('邮箱或密码错误');
    });
  });
});

describe('Password Validation', () => {
  it('should reject weak passwords', async () => {
    const weakPasswords = [
      'short',           // 太短
      'nouppercase123',  // 无大写字母
      'NOLOWERCASE123',  // 无小写字母
      'NoNumbers',       // 无数字
    ];

    for (const password of weakPasswords) {
      await expect(
        authService.register('testuser', 'test@example.com', password)
      ).rejects.toThrow();
    }
  });
});
