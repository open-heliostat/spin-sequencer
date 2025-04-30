<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from '$lib/components/GridForm.svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import Text from './Text.svelte';
    import Slider from './Slider.svelte';
    import Button from './Button.svelte';
    import Checkbox from './Checkbox.svelte';
	import Terminal from '~icons/tabler/terminal'
    import Spinner from './Spinner.svelte';
    import type { CanSettings } from '$lib/types/models';
	import NumberInput from './NumberInput.svelte';

    export let restPath: string;

    let canSettings: CanSettings = {
        enabled: false,
        txId: 0,
        rxId: 0,
        messageHistory: []
    };

    let canMessage: string = '';

    async function getCanSettings() {
        return getJsonRest(restPath, canSettings).then((data) => {
            canSettings = data;
        });
    }
    async function postCanSettings() {
        return postJsonRest(restPath, canSettings).then((data) => {
            canSettings = data;
        });
    }
    async function postCanMessage() {
        return postJsonRest(restPath + "/sendMessage", {message: canMessage, txId: canSettings.txId});
    }

    let intervalID: any;
    onMount(() => {
        intervalID = setInterval(() => {
            getCanSettings();
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
</style>

<SettingsCard>
	<Terminal slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">Can Console</span>
    {#await getCanSettings()}
        <Spinner></Spinner>
    {:then nothing}
    <div class="mt-4 bg-gray-900 text-gray-100 rounded-md p-4 h-64 overflow-y-auto font-mono text-sm autoscrollable-wrapper">
        {#each canSettings.messageHistory.reverse() as message}
            {#if message}
            <div class="py-1">
                <span class="text-gray-500">#</span>
                <span class="ml-2">{message}</span>
            </div>
            {/if}
        {/each}
    </div>
    {/await}
    <Slider
        label="Tx ID"
        min={0}
        max={255}
        step={1}
        bind:value={canSettings.txId}
    />
    <Text
        label="Send Message"
        bind:value={canMessage}
        onChange={postCanMessage}
        on:keyup={e => {
            if (e.key === 'Enter') {
                postCanMessage();
            }
        }}
    />
</SettingsCard>