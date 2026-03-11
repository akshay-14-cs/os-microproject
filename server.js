const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');
const path = require('path');
const app = express();
app.use(cors());
app.use(express.json());
app.post('/api/simulate', (req, res) => {
    const { algorithm, frameCount, refString } = req.body;
    const pages = refString.split(',').map(n => n.trim()).filter(n => n !== "");
    const executablePath = path.join(__dirname, 'simulator.out');
    const args = [algorithm, frameCount.toString(), ...pages];
    console.log(`[Linux Terminal] Running Simulator with args:`, args);
    execFile(executablePath, args, (error, stdout, stderr) => {
        if (error) {
            console.error("Execution error:", error);
            return res.status(500).json({ error: "Failed to run C simulator" });
        }
        try {
            const result = JSON.parse(stdout);
            res.json(result);
        } catch (parseError) {
            console.error("JSON Parse Error. Output was:", stdout);
            res.status(500).json({ error: "Invalid output from C simulator" });
        }
    });
});
const PORT = 3000;
app.listen(PORT, () => {
    console.log(`🚀 WSL Bridge Server running at http://localhost:${PORT}`);
    console.log(`Waiting for browser requests...`);
});