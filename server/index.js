

const express = require('express')

const PORT = 8080

const app = express()



app.get('/', (req, res) => {
    res.sendFile
})

app.listen(PORT, () => {
    console.log(`Helios server listening on ${PORT}`)
})
