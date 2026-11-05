-- clangd runs in the dev container: the host has no Linux headers (sys/epoll.h).
local root = vim.fn.getcwd()

-- Set on FileType so it lands after a global clangd config that lazy-loads on BufReadPre.
vim.api.nvim_create_autocmd("FileType", {
  pattern = { "c", "cpp" },
  once = true,
  callback = function()
    vim.lsp.config("clangd", {
      cmd = {
        "docker", "compose", "run", "--rm", "-T", "server",
        "clangd",
        "--path-mappings=" .. root .. "=/app",
        "--compile-commands-dir=/app/build",
        "--background-index",
        "--clang-tidy",
        "--header-insertion=iwyu",
        "--completion-style=detailed",
        "--fallback-style=llvm",
      },
    })
  end,
})
