<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import Text from './Text.svelte';
    import Spinner from './Spinner.svelte';
    import Terminal from '~icons/tabler/terminal';

    export let restPath: string;

    let espnowState = {
        enabled: false,
        messageHistory: [],
        lastAddress: ''
    };

    let message: string = '';
    let targetAddress: string = '';

    async function getEspnowState() {
        return getJsonRest(restPath, espnowState).then((data) => {
            espnowState = data;
            console.log("ESPNow State: ", espnowState);
        });
    }

    async function sendMessage() {
        return postJsonRest(restPath + "/sendMessage", {
            message: message,
            address: targetAddress
        }).then(() => {
            message = '';
        }).catch((error) => {
            notifications.error(error.message, 500);
        });
    }

    async function getMessageHistory() {
        return getJsonRest(restPath + "/messageHistory", espnowState.messageHistory).then((data) => {
            espnowState.messageHistory = data;
            console.log("ESPNow Message History: ", espnowState.messageHistory);
        });
    }

    let intervalID: any;
    onMount(() => {
        intervalID = setInterval(() => {
            getMessageHistory();
        }, 1122);
    });
    onDestroy(() => {
        clearInterval(intervalID);
    });
</script>

<style>
    .autoscrollable-wrapper {
        overflow: auto;
        max-height: 100%;
        display: flex;
        flex-direction: column-reverse;
    }
    .autoscrollable-wrapper div {
        width: 100%;
        overflow-wrap: break-word;
        word-break: break-all;
    }
</style>

<SettingsCard>
    <Terminal slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">ESPNow Console</span>
    {#await getEspnowState()}
        <Spinner></Spinner>
    {:then nothing}
    <div class="mt-4 bg-gray-900 text-gray-100 rounded-md p-4 h-64 overflow-y-auto font-mono text-sm autoscrollable-wrapper">
        {#each espnowState.messageHistory.reverse() as message}
            {#if message}
            <div class="py-1">
                <span class="text-gray-500">#</span>
                <span class="ml-2">{message}</span>
            </div>
            {/if}
        {/each}
    </div>
    {/await}
    <Text
        label="Target MAC Address"
        bind:value={targetAddress}
    />
    <Text
        label="Message"
        bind:value={message}
        onChange={sendMessage}
    />
</SettingsCard>
