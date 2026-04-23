const { z } = require('zod');

const registerSchema = z.object({
  username: z.string().min(3).max(50).regex(/^[a-zA-Z0-9_]+$/, '用户名只能包含字母、数字和下划线'),
  email: z.string().email('邮箱格式不正确'),
  password: z.string().min(8).max(128).regex(
    /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)/,
    '密码必须包含大小写字母和数字'
  ),
});

const loginSchema = z.object({
  email: z.string().email('邮箱格式不正确'),
  password: z.string().min(1),
});

const deviceAuthSchema = z.object({
  device_name: z.string().min(1).max(100),
  device_type: z.enum(['windows', 'mac', 'linux', 'ios', 'android', 'harmonyos']),
  device_fingerprint: z.string().min(1).max(255),
});

const clipboardItemSchema = z.object({
  content_type: z.enum(['text', 'image', 'file']),
  content: z.string().optional(),
  content_hash: z.string().optional(),
  file_path: z.string().optional(),
  file_size: z.number().optional(),
  is_encrypted: z.boolean().optional(),
});

const syncPullSchema = z.object({
  after_timestamp: z.string().optional(),
  content_type: z.enum(['text', 'image', 'file']).optional(),
  limit: z.number().min(1).max(100).optional().default(50),
});

const backupSchema = z.object({
  backup_name: z.string().min(1).max(200),
});

function validate(schema) {
  return (req, res, next) => {
    try {
      const result = schema.safeParse(req.body);
      if (!result.success) {
        const errors = result.error.errors.map(e => `${e.path.join('.')}: ${e.message}`);
        return res.status(400).json({ error: '参数验证失败', details: errors });
      }
      req.validatedBody = result.data;
      return next();
    } catch (err) {
      return res.status(400).json({ error: '参数验证失败' });
    }
  };
}

module.exports = {
  registerSchema,
  loginSchema,
  deviceAuthSchema,
  clipboardItemSchema,
  syncPullSchema,
  backupSchema,
  validate,
};
