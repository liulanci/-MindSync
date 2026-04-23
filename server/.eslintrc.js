module.exports = {
  env: {
    node: true,
    es2022: true,
    jest: true,
  },
  extends: [
    'eslint:recommended',
    'plugin:node/recommended',
    'prettier',
  ],
  parserOptions: {
    ecmaVersion: 'latest',
    sourceType: 'module',
  },
  plugins: ['node'],
  rules: {
    'no-console': 'warn',
    'no-unused-vars': ['error', { argsIgnorePattern: '^_' }],
    'no-return-await': 'error',
    'no-await-in-loop': 'error',
    'prefer-const': 'error',
    'no-var': 'error',
    'node/no-unpublished-require': 'off',
    'node/no-missing-require': 'off',
    'no-unsafe-optional-chaining': 'error',
    'no-promise-executor-return': 'error',
    'no-template-curly-in-string': 'warn',
    'consistent-return': 'error',
    'array-callback-return': 'error',
    'default-param-last': 'error',
    'no-constructor-return': 'error',
    'no-invalid-this': 'error',
    'no-self-compare': 'error',
    'no-unmodified-loop-condition': 'error',
    'no-unreachable-loop': 'error',
    'no-use-before-define': 'error',
    'require-atomic-updates': 'error',
  },
  overrides: [
    {
      files: ['**/*.test.js', '**/*.spec.js'],
      env: {
        jest: true,
      },
      rules: {
        'no-console': 'off',
      },
    },
  ],
};
